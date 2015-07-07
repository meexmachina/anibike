# Introduction #

ANIBIKE is an electronic system mounted on bicycle wheels. While operating, it projects animations and images on the wheels.
Similar system has been produced already by monkeylectric, but it's seemed like their product is very costly compared to ANIBIKE. Monkeylectric suggested the video-pro version of their system for not less then 2000$!!! As a comparison, ANIBIKE's gross cost will be something like 30$, giving almost the same performance like the video-pro system.
Our system started as a graduation (electrical engineering BCs degree) project suggested by us to our supervisor in Technion (Israeli Institute of Technology, IIT).

# Details #

ANIBIKE system is based on AVR family micro-controller. The system contains two types of boards: main controller board and secondary slave board. The user may connect number of secondary boards to the same main controller. The communication between those boards is conducted throw a single 5 wire bus (which includes power supply).
The system can show up to 4096 different colors on it. The animations and images are downloaded to the system with a designated computer software (called afarsek). The connection used is Bluetooth connection. Currently only windows version of the program exists but soon Mac OSX and Linux version will be also available. We are planning integrating the software on android computers and smartphones as well in the near future.