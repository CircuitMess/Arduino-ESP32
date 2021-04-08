#ifndef SPENCER_WRITESTREAM_HPP
#define SPENCER_WRITESTREAM_HPP


class WriteStream {
public:

	/**
	 * Write a byte to the stream.
	 */
	virtual void write(unsigned char byte) = 0;

	virtual ~WriteStream(){ };

};


#endif //SPENCER_WRITESTREAM_HPP
