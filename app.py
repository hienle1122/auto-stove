from flask import Flask, request, jsonify
import sqlite3
import datetime
import random
import time

app = Flask(__name__)
stoveDB = 'stoveDB.db'

# state: on(1) or off(0)

#post the current state to statetable
#ex: postState/?state=1      => will post 1(on) to statetable in the database
#                               returns the state you just posted
@app.route('/postState/', methods=['GET'])
def post_state():
    conn = sqlite3.connect(stoveDB)  #connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  #make cursor into database (allows us to execute commands)   
    new_state = request.args.get("state", None)
    c.execute('''CREATE TABLE IF NOT EXISTS statetable (dates timestamp, state int);''') # run a CREATE TABLE command
    c.execute('''INSERT INTO statetable VALUES (?,?);''', (datetime.datetime.now(), int(new_state)))  
    conn.commit() #commit commands
    conn.close() #close connection to database put lasertag_main.py
    return new_state

#ex: /getState/     => will return the latest added state, either 0 or 1 
@app.route('/getState/', methods=['GET'])
def get_state():
    conn = sqlite3.connect(stoveDB)  #connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  #make cursor into database (allows us to execute commands)   
    c.execute('''CREATE TABLE IF NOT EXISTS statetable (dates timestamp, state int);''') # run a CREATE TABLE command
    c.execute("SELECT state FROM statetable ORDER BY dates DESC") # newest one at the top
    # c.execute("SELECT state FROM statetable")
    result = c.fetchall()
    ret = result[0][0]
    conn.commit() #commit commands
    conn.close() #close connection to database put lasertag_main.py
    return str(ret)


#get the latest command
#ex: /getCommand/     => will return the latest added command, ex turnOff
@app.route('/getCommand/', methods=['GET'])
def get_command():
    conn = sqlite3.connect(stoveDB)  #connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  #make cursor into database (allows us to execute commands)   
    c.execute('''CREATE TABLE IF NOT EXISTS commandtable (dates timestamp, command text);''') # run a CREATE TABLE command
    c.execute("SELECT command FROM commandtable ORDER BY dates DESC") # newest one at the top
    result = c.fetchall()
    ret = result[0][0]
    conn.commit() #commit commands
    conn.close() #close connection to database put lasertag_main.py
    return str(ret)


#post turnoff command to db
#ex: /turnOff/?command=turnOff   => will post turnOff to commandtable in dababase
#                                   returns the command you just posted
@app.route('/turnOff/', methods=['GET'])
def turn_off():
    conn = sqlite3.connect(stoveDB)  #connect to that database (will create if it doesn't already exist)
    c = conn.cursor()  #make cursor into database (allows us to execute commands)   
    new_command = request.args.get("command", None)
    c.execute('''CREATE TABLE IF NOT EXISTS commandtable (dates timestamp, command text);''') # run a CREATE TABLE command
    c.execute('''INSERT INTO commandtable VALUES (?,?);''', (datetime.datetime.now(), new_command))  
    conn.commit() #commit commands
    conn.close() #close connection to database put lasertag_main.py
    return new_command


@app.route('/', methods=['GET'])
def res():
    return str(0)

@app.route('/getmsg/', methods=['GET'])
def respond():
    # Retrieve the name from url parameter
    name = request.args.get("name", None)

    # For debugging
    print(f"got name {name}")

    response = {}

    # Check if user sent a name at all
    if not name:
        response["ERROR"] = "no name found, please send a name."
    # Check if the user entered a number not a name
    elif str(name).isdigit():
        response["ERROR"] = "name can't be numeric."
    # Now the user entered a valid name
    else:
        response["MESSAGE"] = f"Welcome {name} to our awesome platform!!"

    # Return the response in json format
    return jsonify(response)


@app.route('/post/', methods=['POST'])
def post_something():
    param = request.form.get('name')
    print(param)
    # You can add the test cases you made in the previous function, but in our case here you are just testing the POST functionality
    if param:
        return jsonify({
            "Message": f"Welcome {name} to our awesome platform!!",
            # Add this option to distinct the POST request
            "METHOD": "POST"
        })
    else:
        return jsonify({
            "ERROR": "no name found, please send a name."
        })

# A welcome message to test our server


@app.route('/')
def index():
    return "<h1>Welcome to our server !!</h1>"


if __name__ == '__main__':
    # Threaded option to enable multiple instances for multiple user access support
    app.run(threaded=True, port=5000)
