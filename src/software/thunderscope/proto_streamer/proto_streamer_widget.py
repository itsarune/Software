import pyqtgraph.console as pg_console
from pyqtgraph.Qt.QtWidgets import *

class ProtoStreamerWidget(QWidget):
    def __init__(self, buffer_size=10, *args):
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

        self.registered_buffers = [ThreadSafeBuffer(buffer_size=buffer_size, protobuf_type=arg) for arg in args]

    def refresh(self):
        proto = None
        for buffer in self.registered_buffers:
            try:
                proto = buffer.queue.get_nowait()
                if proto is not None:
                    break
            except queue.Empty as empty:
                continue

        if proto is not None:
            log_str = f"{proto.name}: {proto.SerializeToString()}\n"
            self.console_widget.write(log_str)
