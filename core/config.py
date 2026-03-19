from dataclasses import dataclass

@dataclass
class GlobalConfig:
    host : str = "192.168.0.77"
    port : int = 8000



@dataclass
class ServerConfig:

    connection : bool = True
    clients : int = 5
    data_size : int = 8
    debug : bool = False


@dataclass 
class ClientConfig:
    debug : bool = False