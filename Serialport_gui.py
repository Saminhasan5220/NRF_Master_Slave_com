#!/usr/bin/env python3
"""Script for Tkinter GUI chat client."""
import serial
import time
import math
import tkinter
from threading import Thread

import argparse
connected = False
# construct the argument parse and parse the arguments
ap = argparse.ArgumentParser()
ap.add_argument("-C", "--port", required=True,
                help="COM PORT TO CONNECT")
args = vars(ap.parse_args())
try:
    port = args["port"]#'COM6'  # "/dev/tty.HC-06-DevB"
    print("Connecting to device connected to " + port)
    NANO = serial.Serial(port, 57600, timeout=0.1)
    connected = True
    time.sleep(2)
    print(NANO.readall().decode('utf-8'))
    print("Connected")

except:
    print("Not connected")
    pass

def receive():
    """Handles receiving of messages."""
    while True:
        try:
            reply = NANO.readline()
            reply = reply.decode('utf-8')
            if len(reply) != 0:
                msg_list.insert(tkinter.END, "Received:" + reply)
        except OSError:  # Possibly client has left the chat.
            break


def send(event=None):  # event is passed by binders.
    """Handles sending of messages."""
    msg = my_msg.get()
    my_msg.set("")  # Clears input field.
    data = str(msg) + '\n'
    data = data.encode('utf-8')
    NANO.write(data)
    msg_list.insert(tkinter.END, "Sent:" + msg)

    if msg == "quit":
        top.destroy()
        if connected:
            NANO.close()
        exit()


def on_closing(event=None):
    """This function is to be called when the window is closed."""
    my_msg.set("quit")
    send()
    top.destroy()
    if connected:
        NANO.close()
    exit()

top = tkinter.Tk()
top.title("Chatter")
messages_frame = tkinter.Frame(top)
my_msg = tkinter.StringVar()  # For the messages to be sent.
my_msg.set("Type your messages here.")
scrollbar = tkinter.Scrollbar(messages_frame)  # To navigate through past messages.
# Following will contain the messages.
msg_list = tkinter.Listbox(messages_frame, height=15, width=50, yscrollcommand=scrollbar.set)
scrollbar.pack(side=tkinter.RIGHT, fill=tkinter.Y)
msg_list.pack(side=tkinter.LEFT, fill=tkinter.BOTH)
msg_list.pack()
messages_frame.pack()
entry_field = tkinter.Entry(top, textvariable=my_msg)
entry_field.bind("<Return>", send)
entry_field.pack()
send_button = tkinter.Button(top, text="Send", command=send)
send_button.pack()
top.protocol("WM_DELETE_WINDOW", on_closing)
receive_thread = Thread(target=receive)
receive_thread.daemon = True
receive_thread.start()
tkinter.mainloop()  # Starts GUI execution.
