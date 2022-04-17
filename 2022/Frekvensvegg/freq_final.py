import sounddevice as sd
import numpy as np
import time
import serial

debug = False
# Define some settings
device = 2    # we use my USB sound card device
duration = 0.2  # seconds
fs = 44100    # samples by second

f4 = 50
f3 = 50
f2 = 50
f1 = 50
f0 = 50
freq = 50

ser = serial.Serial()
# for å finne port kjør ls /dev/tty* i terminalen og se etter en som ligner på den under
ser.port = "/dev/ttyACM0"
ser.baudrate = 9600
ser.open();


print("Audio analyzer running")
while True:
    
    # Listen to a bit of sound from the mic, recording it as a numpy array
    audio = sd.rec(int(duration * fs), samplerate=fs, channels=1, device=device,dtype = 'int16')
    time.sleep(1.1*duration)
    audioNorm = audio/32768.0
    # Calculate the Fourier transform of the recorded signal
    #print(myrecording.ravel())
    fourier = np.fft.fft(audioNorm.ravel())
    # Extrat the fundamental frequency from it 
    f_max_index = np.argmax(abs(fourier)) #[:fourier.size/2])
    # Get the scale of frequencies corresponding to the numpy Fourier transforms definition
    freqs = np.fft.fftfreq(len(fourier))
    # And so the actual fundamental frequency detected is
    f_detected = int(abs(freqs[f_max_index]*fs))
    #f_detected = freqs[f_max_index]*fs
    
    #clean up input
    if f_detected > 120 and f_detected < 760:
        
        f4 = f3
        f3 = f2
        f2 = f1
        f1 = f0
        f0 = f_detected
        freq = int((f0+f1+f2+f3+f4)/5)
        
    b = bytes((str(freq) + "\n"),'utf-8')
    if debug:
        print(freq)
    ser.write(b)
