import streamlit as st
import ctypes
import os
import sys
import argparse

import common
from common import fetch_online_user, display_login_status

# Configure app
st.set_page_config(page_title="Chat App", layout="wide")

# Load the compiled C library
if os.name == "nt":  # Windows
    lib = ctypes.CDLL("./client.dll")
else:  # Linux/Mac
    lib = ctypes.CDLL("./client.so")

# Argument parsing
parser = argparse.ArgumentParser(description="Streamlit Socket Client")
parser.add_argument("--server_ip", type=str, default="127.0.0.1", help="Server IP address")
parser.add_argument("--server_port", type=int, default=8080, help="Server port")
args, unknown = parser.parse_known_args(sys.argv[1:])

# Check if socket is already created in session state, if not, create it
if "client_socket" not in st.session_state:
    st.session_state["client_socket"] = lib.socket_create(args.server_ip.encode(), ctypes.c_int(args.server_port))

if "notification_socket" not in st.session_state:
    st.session_state["notification_socket"] = lib.socket_create(args.server_ip.encode(), ctypes.c_int(args.server_port))

# Initialize session state for authentication
if "logged_in" not in st.session_state:
    st.session_state["logged_in"] = False
    st.session_state["username"] = None

def authenticate(username, password):
    return lib.login(st.session_state["client_socket"], username.encode('utf-8'), password.encode('utf-8'))

def register(username, password):
    return lib.register_account(st.session_state["client_socket"], username.encode('utf-8'), password.encode('utf-8'))

# Main content
st.title("Chat Application")

if not st.session_state["logged_in"]:
    # Display Login & Register if not logged in
    st.subheader("Login or Register")

    # Tabs for Login and Register
    tab1, tab2 = st.tabs(["Login", "Register"])

    # Login Tab
    with tab1:
        st.header("Login")
        login_username = st.text_input("Username", key="login_username")
        login_password = st.text_input("Password", type="password", key="login_password")
        login_button = st.button("Login", key="login_button")

        if login_button:
            user_id = authenticate(login_username, login_password)
            if user_id >= 0:  # Successful login
                st.session_state["logged_in"] = True
                st.session_state["username"] = login_username
                st.session_state["user_id"] = user_id  # Store user_id
                st.success("Login successful!")
                st.rerun()
            else:
                st.error("Invalid username or password. Please try again.")

    # Register Tab
    with tab2:
        st.header("Register")
        register_username = st.text_input("Choose a Username", key="register_username")
        register_password = st.text_input("Choose a Password", type="password", key="register_password")
        register_button = st.button("Register", key="register_button")

        if register_button:
            if register_username and register_password:
                if register(register_username,register_password) == 0:
                    st.error("Username already taken. Please choose a different one.")
                else:
                    st.success("Registration successful! You can now log in.")
            else:
                st.error("Both username and password are required.")
else:
    # After login, hide Login/Register and show welcome message or redirect
    st.success(f"Welcome, {st.session_state['username']}!")

    display_login_status()

    fetch_online_user()

    # Optional logout button
    if st.button("Logout "):
        if common.handle_logout() == 0:
            st.error("Can not logout!!!")
        else:
            st.session_state["logged_in"] = False
            st.session_state["username"] = None
            st.session_state["user_id"] = None
            st.rerun()