require 'cgi'
cgi = CGI.new

puts cgi.header
puts "<html><body><pre>"
puts cgi.params
puts "</pre></body></html>"
