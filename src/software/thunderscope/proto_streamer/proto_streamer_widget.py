from software.thunderscope.thread_safe_buffer import ThreadSafeBuffer

import queue
import pyqtgraph.console as pg_console
from pyqtgraph.Qt.QtWidgets import *

class ProtoStreamerWidget(QWidget):
    def __init__(self, proto_types, buffer_size=10):
        QWidget.__init__(self)

        self.console_widget = pg_console.ConsoleWidget()
        self.console_widget.setStyleSheet(
            """
            border: none;
            border-radius: 5px;
            background: #232629;
            """
        )

        self.layout = QVBoxLayout()

        # disable input and buttons
        self.console_widget.input.hide()
        self.console_widget.exceptionBtn.hide()
        self.console_widget.historyBtn.hide()

        # _lastCommandRow is initialized to None, which causes errors
        # when writing to ReplWidget
        self.console_widget.repl._lastCommandRow = 0

        self.registered_buffers = dict()
        for arg in proto_types:
            self.registered_buffers[arg] = ThreadSafeBuffer(buffer_size=buffer_size, protobuf_type=arg)

        self.layout.addWidget(self.console_widget)
        self.setLayout(self.layout)

    def refresh(self):
        proto = None
        for buffer in self.registered_buffers.values():
            proto = buffer.get(block=False, return_cached=False)
            if proto is not None and proto.IsInitialized():
                break

        if proto is not None:
            proto_str = type(proto).__name__ + ":"
            split_proto = str(proto).split("\n")
            if len(split_proto) > 1 and split_proto[0] != "":
                for proto_pieces in split_proto:
                    proto_str += f"\n\t{proto_pieces}"
                proto_str += "\n"

                self.console_widget.repl.write(proto_str)
