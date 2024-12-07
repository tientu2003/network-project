import pandas as pd
import streamlit as st
from ctypes import *
import os
# Load the compiled C library
if os.name == "nt":  # Windows
    lib = CDLL("./client.dll")
else:  # Linux/Mac
    lib = CDLL("./client.so")


def display_login_status():
    # Login status display
    if st.session_state["logged_in"]:
        st.sidebar.success(f"Logged in as: {st.session_state['username']}")
        logout = st.sidebar.button("Logout")
        if logout:
            st.session_state["logged_in"] = False
            st.session_state["username"] = None
            st.switch_page("app.py")
    else:
        st.sidebar.info("Not logged in. Navigate to Login or Register pages.")
        st.write("Please log in or register to access the chat functionalities.")

class onlineuserlist(Structure):
    _fields_ = [
        ('id', c_int),
        ('name', c_char * 30)
    ]

lib.get_online_users.argtypes = [c_int, POINTER(c_int)]   # Pointer to int for size

def fetch_online_user():
    size = c_int(0)

    lib.get_online_users.restype = POINTER(onlineuserlist)
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

    print(online_users)
    #  # Convert the list of dictionaries to a Pandas DataFrame for better display
    # df = pd.DataFrame(online_users)
    #
    #  # Display the DataFrame in Streamlit
    # st.write(df)

def render_notifications(notifications):
    """Render notifications in the top-right corner using a popover."""
    with st.container():
        col1, col2, col3 = st.columns([0.6, 0.2, 0.2])
        with col3:
            if notifications:
                with st.popover("🔔 Notifications"):
                    for notification in notifications:
                        st.write(f"**{notification['type']}**: {notification['message']}")
            else:
                with st.popover("🔔 Notifications", location="top-right", key="notification_popover"):
                    st.write("No new notifications.")


def friend_request_notification(data):
    return 0


def not_group_notification(data):
    return 0

def new_message_notification(data):
    return 0
