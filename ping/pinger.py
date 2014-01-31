#!/usr/bin/python
import re
import sys
import urlparse
from subprocess import Popen, PIPE
from threading import Thread


class Pinger(object):
    def __init__(self, hosts):
        for host in hosts:
            hostname = urlparse.urlparse(host).hostname
            if hostname:
                pa = PingAgent(hostname)
                pa.start()
            else:
                print "ping host %s" % host
                pa = PingAgent(host)
                pa.start()
                continue

class PingAgent(Thread):
    def __init__(self, host):
        Thread.__init__(self)        
        self.host = host

    def run(self):
        p = Popen('ping -c 1 ' + self.host, stdout=PIPE)
        m = re.search('Average = (.*)ms', p.stdout.read())
        if m: print 'Round Trip Time: %s ms -' % m.group(1), self.host
        else: print 'Error: Invalid Response -', self.host


if __name__ == '__main__':
    
    content = []
    for i in range(255):
        ip = "10.8.3.%d" % i
        print ip
        content.append(ip)

#    with open(sys.argv[1]) as f:
#        content = f.readlines() 

    Pinger(content)
