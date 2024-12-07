import pandas as pd
import streamlit as st
from ctypes import *
import random
import os
# Load the compiled C library
if os.name == "nt":  # Windows
    lib = CDLL("./client.dll")
else:  # Linux/Mac
    lib = CDLL("./client.so")

def handle_logout():
    return lib.logout(st.session_state["client_socket"],st.session_state["user_id"])

def display_login_status():
    # Login status display
    if st.session_state["logged_in"]:
        st.sidebar.success(f"Logged in as: {st.session_state['username']}")
        logout = st.sidebar.button("Logout")
        if logout:
            if handle_logout() == 0:
                st.error("Can not logout!!!")
            else:
                st.session_state["logged_in"] = False
                st.session_state["username"] = None
                st.session_state["user_id"] = None
                st.switch_page("Home.py")
    else:
        st.sidebar.info("Not logged in. Navigate to Login or Register pages.")
        st.write("Please log in or register to access the chat functionalities.")

class Onlineuserlist(Structure):
    _fields_ = [
        ('id', c_int),
        ('name', c_char * 30)
    ]
def fetch_online_user():
    size = c_int(0)
    lib.get_online_users.argtypes = [c_int, POINTER(c_int)]   # Pointer to int for size
    lib.get_online_users.restype = POINTER(Onlineuserlist)
    res = lib.get_online_users(st.session_state['client_socket'],byref(size))
    # Process the result
    online_users = []
    for i in range(size.value):
        user = res[i]  # Access each struct in the array
        user_data = {
            'id': user.id,                        # Access 'id' field
            'name': user.name.decode('utf-8')     # Decode 'name' field (C string to Python string)
        }
        online_users.append(user_data)
    # Convert the list of dictionaries to a Pandas DataFrame for better display
    df = pd.DataFrame(online_users)
    # title
    st.title("Online User List")

    # Add search bar for filtering names
    search_query = st.text_input("Search users by name:", "")
    if search_query:
        df = df[df['name'].str.contains(search_query, case=False)]


    with st.container():
        col1, col2, col3 = st.columns([0.3, 0.4, 0.3])
        with col2:
        # Display the filtered list of users
            st.subheader("User Table")
            st.dataframe(df, height=350, width=500, hide_index= True)  # Scrollable dataframe

def send_message(room_id,content):
    content_ctypes = c_char_p(content.encode('utf-8'))
    return lib.send_message(st.session_state['client_socket'],st.session_state['user_id'],room_id,content_ctypes)

# Function to generate a consistent random color based on a user's name
def generate_color_from_name(user_name):
    # Use the hash of the user's name to generate a numeric value
    hash_value = hash(user_name)

    # Use the hash value to generate a RGB color by manipulating the value
    random.seed(hash_value)  # Seed the random number generator for consistency
    color = "#{:02x}{:02x}{:02x}".format(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

    return color