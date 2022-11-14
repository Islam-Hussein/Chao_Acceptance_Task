import serial
import matplotlib.pyplot as plt

# If you are using MS windows, you should uncomment the line below
# and change the serial device accordingly
ser = serial.Serial("COM9", 115200)

# If you are using Unix, you should uncomment the line below
# and change the serial device accordingly
#ser = serial.Serial("/dev/ttyACM0", 500000)


if __name__ == '__main__':
	#plt.ion()

	while True:
		#ser.write(b'1')
		data_rd_byte_array = (ser.readline())
		data_as_string_list = data_rd_byte_array.decode("utf-8").split(',')[:-1]

		data_as_string_list = data_rd_byte_array.decode("utf-8")

		data_as_int_list = [int(i) for i in data_as_string_list]
		print((len(data_as_int_list)))
		print(data_as_string_list)

		plt.plot(int(data_as_string_list))
		plt.pause(0.0001) # not necessary atm
		plt.clf()
		plt.ylim(0, 4)

