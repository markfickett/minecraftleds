import sys
import logging
import os
import time

base_dir = os.path.dirname(__file__)
# Expect to be in the Arduino sketchbook, where DataReceiver is a library.
# https://github.com/markfickett/datareceiver
sys.path.append(
    os.path.abspath(
        os.path.join(base_dir, '..', 'libraries', 'DataReceiver')))
# Use `git submodule update --init` if the submodule is empty.
sys.path.append(os.path.join(base_dir, 'minecraftstatus'))

from mcstatus import McServer
from data_sender import Sender


_KNOWN_PLAYERS = frozenset((
  'CapybaraRancher',
  'Wickedhound',
  'krc23',
  'markfickett',
))
_UNKNOWN_PLAYER = 'other'
_AVAILABLE = 'available'

_SERVER_ADDRESS = 'naib'


if __name__ == '__main__':
  logging.basicConfig(
    format='%(levelname)s %(asctime)s %(filename)s:%(lineno)s: %(message)s',
    level=logging.DEBUG)
  logging.info('Preparing to run LEDs.')

  server = McServer(_SERVER_ADDRESS)
  server.Update()
  logging.info(
      '%s is %savailable.', _SERVER_ADDRESS, '' if server.available else 'not')

  sender = Sender('/dev/ttyACM0', start_ready=True, read_timeout=0.05)

  with sender:
    logging.info('Communicating with Arduino.')
    try:
      last_status = None
      while True:
        time.sleep(1.0)
        server.Update()
        status = {
            name: name in server.player_names_sample
            for name in _KNOWN_PLAYERS}
        status[_UNKNOWN_PLAYER] = bool(
            server.player_names_sample - _KNOWN_PLAYERS)
        status[_AVAILABLE] = server.available
        if status != last_status:
          logging.info(status)
          last_status = status
        sender.Send(**status)
        sender.ReadAndPrint()
    except KeyboardInterrupt:
      logging.info('Clearing LEDs and exiting.')
      final_status = {name: False for name in _KNOWN_PLAYERS}
      final_status[_UNKNOWN_PLAYER] = False
      sender.Send(**final_status)
