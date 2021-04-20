#ifndef JAYD_OUTPUTI2S_H
#define JAYD_OUTPUTI2S_H

#include "Output.h"
#include <driver/i2s.h>
class OutputI2S : public Output
{
public:
	OutputI2S(i2s_config_t config, i2s_pin_config_t pins, i2s_port_t port = I2S_NUM_0);
	~OutputI2S();

	void init() override;
	void deinit() override;

protected:
	void output(size_t numBytes) override;

private:
	i2s_config_t config;
	i2s_pin_config_t pins;
	i2s_port_t port;
};


#endif