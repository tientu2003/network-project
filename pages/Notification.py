from ctypes import c_int

import streamlit as st
import ctypes
import os
# Load the compiled C library
if os.name == "nt":  # Windows
    lib = ctypes.CDLL("./client.dll")
else:  # Linux/Mac
    lib = ctypes.CDLL("./client.so")

# Define the C structs as Python classes using ctypes

class NotiData(ctypes.Structure):
    _fields_ = [("sender", ctypes.c_int),
                ("type", ctypes.c_int),
                ("target", ctypes.c_int),
                ("name", ctypes.c_char * 30)]  # name will be a byte string

# Define function prototypes for the C functions

# For `get_notifications`
lib.get_notifications.restype = ctypes.POINTER(NotiData)

lib.send_friend_cmd.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
lib.send_friend_cmd.restype = ctypes.c_int


lib.send_group_cmd.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
lib.send_group_cmd.restype = ctypes.c_int

# Handle the response for each notification (e.g., sending the sender_id)
def handle_accept(sender_id, t, target):
    if t == 0:
        lib.send_friend_cmd(st.session_state['client_socket'], 3, sender_id, st.session_state['user_id'], 1)
    else:
        lib.send_group_cmd(st.session_state['client_socket'], 4, sender_id, st.session_state['user_id'], target, 1)
    st.rerun()
def handle_decline(sender_id, t, target):
    if t == 0:
        lib.send_friend_cmd(st.session_state['client_socket'], 3, sender_id, st.session_state['user_id'], 0)
    else:
        lib.send_group_cmd(st.session_state['client_socket'], 4, sender_id, st.session_state['user_id'],target,0 )
    st.rerun()

def display_notification(notif):
    notification_type = "Friend Request" if notif.type == 0 else "Group Invitation"
    sender_name = notif.name.decode("utf-8").strip("\x00")
    st.write(f"**{notification_type}** from {sender_name}")

    # Create two columns for the buttons
    col1, col2 = st.columns([1, 1])

    with col1:
        # Accept button
        if st.button("Accept", key=f"accept_{notif.sender}"):
            st.success(f"Accepted {notification_type} from {sender_name}")
            handle_accept(notif.sender, notif.type, notif.target)  # Call the accept function

    with col2:
        # Decline button
        if st.button("Decline", key=f"decline_{notif.sender}"):
            st.error(f"Declined {notification_type} from {sender_name}")
            handle_decline(notif.sender, notif.type,notif.target)  # Call the decline function

if not st.session_state["logged_in"]:
    st.warning("You must log in to access the chat rooms!")
else:
    size = c_int(0)
    notifications = lib.get_notifications(st.session_state['client_socket'], st.session_state['user_id'], ctypes.byref(size))
    # Main page layout
    st.title("Notifications")
    st.write("Here are your notifications:")

    for index in range(size.value):
        with st.container(border=True):
            display_notification(notifications[index])