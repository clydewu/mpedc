#!/bin/bash

EDC_IP=$1
ID_RSA_PUB=`cat ~/.ssh/id_rsa.pub`

ssh root@$EDC_IP "mkdir -p ~/.ssh/; echo \"RSAAuthentication yes\n PubkeyAuthentication yes  \" >> /etc/sshd_config; /etc/init.d/echo $ID_RSA_PUB >> ~/.ssh/authorized_keys"
