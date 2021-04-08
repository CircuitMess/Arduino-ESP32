#ifndef SPENCER_READSTREAM_HPP
#define SPENCER_READSTREAM_HPP


class ReadStream {
public:

	/**
	 * Whether bytes are available in the stream.
	 * @return True if yes, false if no.
	 */
	virtual bool available() = 0;

	/**
	 * Get the next byte in the stream.
	 * @return The next byte, or 0 if no bytes are available.
	 */
	virtual unsigned char get() = 0;

	virtual ~ReadStream(){ };

};


#endif //SPENCER_READSTREAM_HPP
