from random import randint
import asyncio
import socket
import json
import random
import websockets
import time

run = True
print('start')
async def web_socket(websocket,path):
    global data
    global times
    times = 0
    while(run):
        num = randint(360,390)
        num = num/10
        times += 1
        print(num)
        print(times)
        data = "{\"val\":%d , \"time\":%d}" % (num,times)
        #data = "{\"time\":%d}" % (times)
        time.sleep(1)
        await websocket.send(data)
    
start_server = websockets.serve(web_socket,"127.0.0.1",5050)
asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
        