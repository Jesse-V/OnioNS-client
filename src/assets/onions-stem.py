
import stem, stem.connection, stem.socket
from stem.control import EventType, Controller

import errno # https://stackoverflow.com/questions/14425401/
from socket import error as socket_error

import socket, functools, re, sys
from threading import Thread

import time, datetime

# start of application
def main():

  print 'Opening log file, further output will be there.'

  # redirect output to file, https://stackoverflow.com/questions/7152762
  f = file('TorBrowser/OnioNS/stem.log', 'w')
  sys.stdout = f

  # get current time of day
  now = datetime.datetime.now()

  try:
    # open main controller
    controller = Controller.from_port(port = 9151)
  except stem.SocketError:
    sys.exit("[err] The Tor Browser is not running. Cannot continue")

  controller.authenticate()
  controller.set_options({
    '__LeaveStreamsUnattached': '1'
  })

  print '[%d:%d | notice] Successfully connected to the Tor Browser.' % (now.minute, now.second)
  sys.stdout.flush()

  event_handler = functools.partial(handle_event, controller)
  controller.add_event_listener(event_handler, EventType.STREAM)

  print '[%d:%d | debug ] Now monitoring stream connections.' % (now.minute, now.second)
  sys.stdout.flush()

  try:
    time.sleep(60 * 60 * 24 * 365) #basically, wait indefinitely
  except KeyboardInterrupt:
    print ''



# handle a stream event
def handle_event(controller, stream):
  p = re.compile('.*\.tor$', re.IGNORECASE)
  if p.match(stream.target_address) is not None: # if .tor, send to OnioNS
    t = Thread(target=resolveOnioNS, args=[controller, stream])
    t.start()
  elif stream.circ_id is None: # if not .tor and unattached, attach now
    attachStream(controller, stream)

  # print '[debug] Finished handling stream.'



# resolve via OnioNS a stream's destination
def resolveOnioNS(controller, stream):
  now = datetime.datetime.now()

  print '[%d:%d | notice] Detected OnioNS domain!' % (now.minute, now.second)
  sys.stdout.flush()

  # send to OnioNS and wait for resolution
  # https://docs.python.org/2/howto/sockets.html
  ipc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

  try:
    ipc.connect(('127.0.0.1', 9053))
    ipc.send(stream.target_address)
    dest = ipc.recv(64) # OnioNS returns an invalid hostname on fail
    ipc.close()
  except socket_error as serr:
    if serr.errno != errno.ECONNREFUSED:
      raise serr

    now = datetime.datetime.now()
    print '[%d:%d | warn  ] OnioNS client is not running!' % (now.minute, now.second)
    dest = '<IPC_FAIL>'

  r=str(controller.msg('REDIRECTSTREAM ' + stream.id + ' ' + dest))
  print '[notice] Rewrote ' + stream.target_address + ' to ' + dest + ', ' + r
  sys.stdout.flush()

  attachStream(controller, stream)



# attach the stream to some circuit
def attachStream(controller, stream):
# print '[debug] Attaching request for ' + stream.target_address + ' to circuit'

  try:
    controller.attach_stream(stream.id, 0)
  except stem.UnsatisfiableRequest:
    pass

  sys.stdout.flush()


if __name__ == '__main__':
  main()
