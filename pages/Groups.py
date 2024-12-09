import pandas as pd
import streamlit as st
import ctypes

import os
# Load the compiled C library
if os.name == "nt":  # Windows
    lib = ctypes.CDLL("./client.dll")
else:  # Linux/Mac
    lib = ctypes.CDLL("./client.so")

class Group(ctypes.Structure):
    _fields_ = [("id", ctypes.c_int),
                ("name", ctypes.c_char * 500)]  # name will be a byte string

# For `get_friend_list`
lib.get_group_list.restype = ctypes.POINTER(Group)

lib.send_group_cmd.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int]
lib.send_group_cmd.restype = ctypes.c_int

if not st.session_state["logged_in"]:
    st.warning("You must log in to access the chat rooms!")
else:
    def create_group():
        if lib.send_group_cmd(st.session_state['client_socket'], 1, st.session_state['user_id'], 0, 0, 0) == 0:
            st.toast("Create New Group Succesfully")
            st.rerun()
        else:
            st.toast("Failed to create new group!!")

    def add_group(receiver, room_id):
        if lib.send_group_cmd(st.session_state['client_socket'], 2, st.session_state['user_id'], receiver,room_id, 0) == 0:
            st.toast("Send Group Invitation Succesfully")
        else:
            st.toast("Failed to send group invitation!!")

    def leave_group(room_id):
        if lib.send_group_cmd(st.session_state['client_socket'], 3, st.session_state['user_id'], 0 ,room_id , 0) == 0:
            st.toast("Remove Friend Succesfully")
            st.rerun()
        else:
            st.toast("Failed to remove friend!!")

    def remove_member(receiver, room_id):
        if lib.send_group_cmd(st.session_state['client_socket'], 3, receiver, 0, room_id , 0) == 0:
            st.toast("Remove Friend Succesfully")
            st.rerun()
        else:
            st.toast("Failed to remove friend!!")


    # Streamlit interface
    st.title("Group Management")
    size = ctypes.c_int(0)

    # Fetch group list
    res = lib.get_group_list(st.session_state['client_socket'], st.session_state['user_id'], ctypes.byref(size))

    # Convert the results into a list of dictionaries for easier processing
    groups_list = []
    for i in range(size.value):
        group = res[i]  # Access each Group struct
        groups_list.append({"ID": group.id, "Name": group.name.decode("utf-8").strip()})

    # Convert the list into a DataFrame
    df = pd.DataFrame(groups_list)

    # Display the DataFrame as a table in Streamlit
    with st.container():
        col1, col2, col3 = st.columns([0.3, 0.4, 0.3])
        with col2:
            st.dataframe(df, height=350, width=500, hide_index= True)  # Scrollable dataframe



    # Interaction Buttons
    with st.container():
        col1, col2, col3, col4 = st.columns(4)

        with col1:
            st.subheader("Create New Group")
            if st.button("Create Group"):
                create_group()

        with col2:
            st.subheader("Add Member to Group")
            group_id_to_add = st.number_input("Group ID", min_value=0, step=1, key="add_group_id")
            receiver_id = st.number_input("Receiver ID", min_value=0, step=1, key="add_member_receiver_id")
            if st.button("Add Member"):
                add_group(receiver_id, group_id_to_add)

        with col3:
            st.subheader("Leave Group")
            group_id_to_leave = st.number_input("Group ID", min_value=0, step=1, key="leave_group_id")
            if st.button("Leave Group"):
                leave_group(group_id_to_leave)

        with col4:
            st.subheader("Remove Member from Group")
            group_id_to_remove = st.number_input("Group ID", min_value=0, step=1, key="remove_group_id")
            member_id = st.number_input("Member ID", min_value=0, step=1, key="remove_member_id")
            if st.button("Remove Member"):
                remove_member(member_id, group_id_to_remove)
