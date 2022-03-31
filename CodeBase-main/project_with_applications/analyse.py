# getting data from onem2m and analysing it

from requests.api import get
from onem2m import *
from matplotlib import pyplot as plt
import numpy as np
import json
from Sendmail import Send_Mail
import time
uds = "Ultrasonic_Distance_sensor"
pir = "PIR_motion_sensor"
ldr = "LDR_sensor"

DISTANCE_LIMIT = 350

uri_cse = "http://127.0.0.1:8080/~/in-cse/in-name"

headers = {
    'X-M2M-Origin': 'admin:admin',
    'Content-type': 'application/json'
}


def get_recent_data(ae, node_number):
    uri_cnt = uri_cse + "/" + ae + "/node"+str(node_number)
    uri = uri_cnt+"/la"
    # print(uri)
    response = requests.get(uri, headers=headers)
    # print(response.text)
    result = json.loads(response.text)
    y = float(result["m2m:cin"]["con"])
    return y


def get_data_from_onem2m(ae, node_number):
    uri_cnt = uri_cse + "/" + ae + "/node"+str(node_number)
    uri = uri_cnt+"/?rcn=4"
    # print(uri)
    response = requests.get(uri, headers=headers)
    # print(response.text)
    result = json.loads(response.text)

    x = []
    y = []
    if result["m2m:cnt"].get("m2m:cin") is not None:
        x = [i["ct"] for i in result["m2m:cnt"]["m2m:cin"]]
        y = [float(i["con"]) for i in result["m2m:cnt"]["m2m:cin"]]
    return x, y


def plot_data(x, y, ae, n):

    plt.xlabel("time")

    plt.ylabel((ae.split("_", 1))[0])

    plt.title(ae + " at node "+str(n))

    plt.bar(x, y)

    # hide tick lables
    plt.xticks([])
    # plt.yticks([])

    plt.show()


# code below

# x, y = get_data_from_onem2m(uds, 1)
# plot_data(x, y, uds, 1)


# x, y = get_data_from_onem2m(ldr, 1)
# plot_data(x, y, ldr, 1)

# x, y = get_data_from_onem2m(pir, 1)
# plot_data(x, y, pir, 1)


while True:
    d = get_recent_data(uds, 1)
    if d is not None and d < DISTANCE_LIMIT:  # 50 cm limit
        print("distance = ", d)
        Send_Mail("Krishna.p.2020.2002@gmail.com",
                  f"Distance of an object or vehicle behind you is {d} cm which is less than MINIMUM LIMIT DISTANCE OF {DISTANCE_LIMIT}")

    time.sleep(600)  # every 10 minutes

# x, y = get_data_from_onem2m(uds, 1)
# for i, j in zip(x, y):
#     print(i, j)

# plot_data(x, y, uds, 1)

# x, y = get_data_from_onem2m(uds, 2)
# for i, j in zip(x, y):
#     print(i, j)


# x, y = get_data_from_onem2m(pir, 1)
# for i, j in zip(x, y):
#     print(i, j)

# x, y = get_data_from_onem2m(pir, 2)
# for i, j in zip(x, y):
#     print(i, j)


# x, y = get_data_from_onem2m(ldr, 1)
# for i, j in zip(x, y):
#     print(i, j)

# x, y = get_data_from_onem2m(ldr, 2)
# for i, j in zip(x, y):
#     print(i, j)
