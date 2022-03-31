# creating containers and nodes in onem2m server

from onem2m import *
uri_cse = "http://127.0.0.1:8080/~/in-cse/in-name"


def create_ae_with_n_nodes(ae, number_of_nodes):
    '''ae ( application entity ) is for sensor name.'''
    create_ae(uri_cse, ae)
    for i in range(1, number_of_nodes+1):
        # create container
        uri_ae = uri_cse + "/" + ae
        create_cnt(uri_ae, "node"+str(i))


create_ae_with_n_nodes("Ultrasonic_Distance_sensor", 2)
create_ae_with_n_nodes("PIR_motion_sensor",2)
create_ae_with_n_nodes("LDR_sensor",2)
