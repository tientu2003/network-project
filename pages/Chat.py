import streamlit as st
import os
from streamlit_autorefresh import st_autorefresh
import common
import ctypes
if os.name == "nt":  # Windows
    lib = ctypes.CDLL("./client.dll")
else:  # Linux/Mac
    lib = ctypes.CDLL("./client.so")
class Mess(ctypes.Structure):
    _fields_ = [
        ('sender', ctypes.c_char * 30),
        ('content',ctypes.c_char * 1024)
    ]

lib.get_history_messages.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]
lib.get_history_messages.restype = ctypes.POINTER(Mess)
def fetch_messages(room_id):
    size = ctypes.c_int(0)
    res = lib.get_history_messages(
        st.session_state['client_socket'],
        room_id,
        ctypes.byref(size)
    )
    messages = []
    for i in range(size.value):
        message = res[i]
        messages.append({
            "sender": message.sender.decode('utf-8'),
            "content": message.content.decode('utf-8')
        })
    return messages
# Define the Rooms structure
class Rooms(ctypes.Structure):
    _fields_ = [
        ('id', ctypes.c_int),
        ('name', ctypes.c_char * 256)
    ]

# Configure argument and return types for the function
lib.get_room_list.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_int)]  # Pointer to int for size
lib.get_room_list.restype = ctypes.POINTER(Rooms)

if not st.session_state["logged_in"]:
    st.warning("You must log in to access the chat rooms!")
else:

    # Call the library function to get the room list
    size = ctypes.c_int(0)
    res = lib.get_room_list(
        st.session_state['client_socket'],
        st.session_state['user_id'],
        ctypes.byref(size)
    )

    chat_rooms = []
    user_name_str = str(st.session_state['username'])

    # Extract room details and append to the chat_rooms list
    for i in range(size.value):
        room = res[i]

        raw_name = room.name.decode('utf-8')
        # Remove all occurrences of user_id from the raw_name
        clean_name = raw_name.replace(user_name_str, "").strip()

        room_data = {
            'id': room.id,
            'name': clean_name  # Decode the C string to Python string
        }
        chat_rooms.append(room_data)

    # Sidebar for chat room selection
    with st.sidebar:
        selected_room = st.selectbox(
            "Select a Chat Room",
            options=[room['name'] for room in chat_rooms],
            format_func=lambda name: f"Room: {name}"
        )

    common.display_login_status()

    # Ensure selected_room matches exactly with stored keys in chat_history
    st.title(f"Chat: {selected_room}")

    # Store the selected room ID
    selected_room_id = next(room['id'] for room in chat_rooms if room['name'] == selected_room)
    st.session_state['selected_room_id'] = selected_room_id


    # Initialize chat history for the selected room
    if "chat_history" not in st.session_state:
        st.session_state.chat_history = {room['name']: [] for room in chat_rooms}

    # Check if the selected room exists in chat history
    if selected_room not in st.session_state.chat_history:
        st.session_state.chat_history[selected_room] = []  # Initialize if not found

    st.session_state.chat_history[selected_room] = fetch_messages(selected_room_id)

    # Tự động refresh UI mỗi 2 giây
    st_autorefresh(interval=2000)

    # Display chat messages
    for message in st.session_state.chat_history[selected_room]:
        color = common.generate_color_from_name(message['sender'])
        sender = message["sender"]
        with st.chat_message(sender):
            st.markdown(f'<span style="color:{color}; font-weight:bold;">{sender}:</span> {message["content"]}', unsafe_allow_html=True)

    # Input and send message
    if user_input := st.chat_input("Type your message here..."):
        if common.send_message(selected_room_id,user_input) == 0:
            # Add user message
            st.session_state.chat_history[selected_room].append({"sender": {st.session_state['username']}, "content": user_input})
            with st.chat_message(st.session_state['username']):
                sender = st.session_state['username']
                color = common.generate_color_from_name(sender)
                st.markdown(f'<span style="color:{color}; font-weight:bold;">{sender}:</span> {user_input}', unsafe_allow_html=True)
        else:
            st.toast("Failed to send message!!")




