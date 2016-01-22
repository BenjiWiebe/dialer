#!/usr/bin/ruby
require 'socket'

MODEM = "/dev/ttyACM0"
area="732"

thread = Thread.new {}
server = TCPServer.new 2893
loop do
	client = server.accept
	if thread.status
		puts "Dialer busy."
		client.close
		next
	end
	thread = Thread.new do
		number = client.gets
		client.close
		Thread.exit if number.nil?
		number.chomp!
		Thread.exit if number.length == 0
		if number !~ /^\d{7,13}$/
			puts "#{number} is not a valid phone number."
			Thread.exit
		end
		if number[-7..-5] == area
			puts "Local call"
			number=number[-7..-1]
		end
		puts "Dialing #{number}"
		File.open(MODEM, "w") do |m|
			m.write "atdt#{number}\r\n"
			puts "Dialing..."
			sleep 6
			puts "Hanging up."
			m.write "ath0\r\n"
		end
		Thread.exit
	end
end
