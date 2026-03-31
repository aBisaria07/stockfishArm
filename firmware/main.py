import serial
import chess
import chess.engine
import time

SERIAL_PORT = '/dev/ttyUSB0' 
STOCKFISH_PATH = "/usr/games/stockfish"

board = chess.Board()
ser = serial.Serial(SERIAL_PORT, 9600, timeout=1)
engine = chess.engine.SimpleEngine.popen_uci(STOCKFISH_PATH)
lifted_sq = None

def send_to_arm(move_uci):
    print(f"Robot moving: {move_uci}")
    ser.write(f"{move_uci}\n".encode())
    while True: 
        if ser.readline().decode().strip() == "DONE":
            break

def process_move(m_str):
    move = chess.Move.from_uci(m_str)
    if move in board.legal_moves:
        board.push(move)
        
        result = engine.play(board, chess.engine.Limit(time=1.0))
        
        
        if board.piece_at(result.move.to_square):
            victim = chess.square_name(result.move.to_square)
            send_to_arm(f"{victim}z9") 

        send_to_arm(result.move.uci())
        board.push(result.move)
        print(board)
    else:
        print("Illegal move detected!")

print("System Live. Make a move on the board.")
while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode().strip()
        if line.startswith("UPDATE"):
            _, sq, status = line.split(":")
            if status == "EMPTY": lifted_sq = sq
            elif status == "FULL" and lifted_sq:
                process_move(lifted_sq + sq)
                lifted_sq = None
