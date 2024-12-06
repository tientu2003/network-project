import streamlit as st
from datetime import datetime

# Initialize session state to store chat messages
if "messages" not in st.session_state:
    st.session_state.messages = []

# Sidebar for user login
with st.sidebar:
    st.header("Login")
    username = st.text_input("Enter your name", key="username")

# Main app
st.title("Simple Chat App")

if username:
    # Display chat messages
    st.header("Chat Room")
    chat_area = st.empty()

    if st.session_state.messages:
        messages_str = "\n".join(
            [f"{msg['time']} - {msg['user']}: {msg['text']}" for msg in st.session_state.messages]
        )
        chat_area.text_area("Chat Log", messages_str, height=300, disabled=True)

    # Input area for sending messages
    message = st.text_input("Type your message", key="new_message")

    if st.button("Send"):
        if message.strip():
            # Append the new message to the session state
            st.session_state.messages.append({
                "user": username,
                "text": message.strip(),
                "time": datetime.now().strftime("%H:%M:%S"),
            })
            # Clear the input box
            st.session_state.new_message = ""
            st.experimental_rerun()
else:
    st.warning("Please log in using the sidebar to start chatting.")
    