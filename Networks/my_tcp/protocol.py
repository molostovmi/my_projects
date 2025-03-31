import socket
import threading
from collections import deque
from select import select
from json import dumps, loads
from base64 import b64encode, b64decode
import time

class UDPBasedProtocol:
    def __init__(self, *, local_addr, remote_addr):
        self.udp_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        self.remote_addr = remote_addr
        self.udp_socket.bind(local_addr)

    def sendto(self, data):
        return self.udp_socket.sendto(data, self.remote_addr)

    def recvfrom(self, n):
        msg, addr = self.udp_socket.recvfrom(n)
        return msg

    def close(self):
        self.udp_socket.close()


class MyTCPProtocol(UDPBasedProtocol):
    size_of_packet = 65536
    size_of_segment = 32768

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self.sended = 0
        self.delivered = 0
        self.received = 0
        self.next = 0
        self.send_buffer = deque()
        self.receive_buffer = dict()

        self.is_run = True
        self.daemon_thread = threading.Thread(target=self._connection, daemon=True)
        self.daemon_thread.start()

    def _can_read(self):
        result, _, _ = select([self.udp_socket], [], [], 0.0001)
        return bool(result)

    def _window_empty(self):
        return self.sended >= self.delivered + 32 or self.sended >= self.delivered + len(self.send_buffer)

    def _connection(self):
        while self.is_run:
            do_answer_needed = False

            while self._can_read():
                do_answer_needed = self._process_packet(self.recvfrom(self.size_of_packet)) or do_answer_needed

            if not self._window_empty() or do_answer_needed:
                self._send_packet()
                continue

            start_waiting_time = time.time()
            old_buffer_size = len(self.send_buffer)

            while time.time() - start_waiting_time < 0.035:
                if self._can_read() or len(self.send_buffer) > old_buffer_size:
                    break
            else:
                if self.delivered < self.sended:
                    self._send_packet(True)

    def _send_packet(self, resend=False):
        if self._window_empty():
            resend = True

        data = b''
        segment_index = -1

        if self.send_buffer:
            segment_index = self.delivered if resend else self.sended
            data = self.send_buffer[segment_index - self.delivered]

        packet = {
            "seg": segment_index,
            "rec": self.received,
            "data": b64encode(data).decode()
        }

        self.sendto(dumps(packet).encode())
        self.sendto(dumps(packet).encode())

        if segment_index == self.sended:
            self.sended += 1

    def _process_packet(self, data):
        packet = loads(data)
        do_need_answer = False

        if packet["seg"] >= self.received:
            do_need_answer = True
            self.receive_buffer[packet["seg"]] = b64decode(packet["data"].encode())

            while self.received in self.receive_buffer:
                self.received += 1

        if packet["rec"] > self.delivered:
            while self.delivered < packet["rec"]:
                self.send_buffer.popleft()
                self.delivered += 1

        return do_need_answer

    def send(self, data):
        data_segments = [data[i:i + self.size_of_segment] for i in range(0, len(data), self.size_of_segment)]
        self.send_buffer.extend(data_segments)

        return len(data)

    def recv(self, count):
        data_segments_count = (count // self.size_of_segment) + (1 if count % self.size_of_segment != 0 else 0)

        while self.received < self.next + data_segments_count:
            time.sleep(0.000001)

        result = b''.join(self.receive_buffer.pop(i) for i in range(self.next, self.next + data_segments_count))

        self.next += data_segments_count
        return result

    def close(self):
        self.is_run = False
        self.daemon_thread.join()
        super().close()