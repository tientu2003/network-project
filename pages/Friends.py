import pandas as pd
import streamlit as st
import ctypes

import os
# Load the compiled C library
if os.name == "nt":  # Windows
    lib = ctypes.CDLL("./client.dll")
else:  # Linux/Mac
    lib = ctypes.CDLL("./client.so")

class Friend(ctypes.Structure):
    _fields_ = [("id", ctypes.c_int),
                ("name", ctypes.c_char * 30)]  # name will be a byte string

# For `get_friend_list`
lib.get_friend_list.restype = ctypes.POINTER(Friend)

lib.send_friend_cmd.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
lib.send_friend_cmd.restype = ctypes.c_int
if not st.session_state["logged_in"]:
    st.warning("You must log in to access the chat rooms!")
else:
    # Function to add a friend
    def add_friend(friend_id):
        if lib.send_friend_cmd(st.session_state['client_socket'], 1, st.session_state['user_id'],int(friend_id), 0) == 0:
            st.toast("Create Friend Request Succesfully")
        else:
            st.toast("Failed to create Friend Request!!")


    # Function to remove a friend
    def remove_friend(friend_id):
        if lib.send_friend_cmd(st.session_state['client_socket'], 2, st.session_state['user_id'],int(friend_id), 0) == 0:
            st.toast("Remove Friend Succesfully")
            st.rerun()
        else:
            st.toast("Failed to remove friend!!")

    # Streamlit interface
    st.title("Friend Management")
    size = ctypes.c_int(0)
    # Display current friends
    res = lib.get_friend_list(st.session_state['client_socket'],st.session_state['user_id'],ctypes.byref(size))

    # Convert the results into a list of dictionaries for easier processing
    friends_list = []
    for i in range(size.value):
        friend = res[i]  # Access each Friend struct
        friends_list.append({"ID": friend.id, "Name": friend.name.decode("utf-8").strip()})

    # Convert the list into a DataFrame
    df = pd.DataFrame(friends_list)

    # Display the DataFrame as a table in Streamlit
    with st.container():
        col1, col2, col3 = st.columns([0.3, 0.4, 0.3])
        with col2:
            st.dataframe(df, height=350, width=500, hide_index= True)  # Scrollable dataframe

    # Input to add a friend
    new_friend = st.text_input("Enter the id of the friend to add:")
    if st.button("Add Friend"):
        add_friend(new_friend)


    # Input to remove a friend
    friend_to_remove = st.text_input("Enter the id of the friend to remove:")
    if st.button("Remove Friend"):
        remove_friend(friend_to_remove)
