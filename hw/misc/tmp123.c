/* 
 * A Hypothetical temperature sensor similar to
 * TMP105
 */

#include "qemu/osdep.h"
#include "hw/i2c/i2c.h"
#include "hw/irq.h"
#include "migration/vmstate.h"
#include "tmp123.h"
#include "qapi/error.h"
#include "qapi/visitor.h"
#include "qemu/module.h"
#include "qemu/log.h"

static const int tmp123_faultq[4] = {1, 2, 4, 6};

static char* get_timestamp(void)
{
	time_t clk;
	time(&clk);

	return ctime(&clk);
}
/* 
 * Trace of below call.
 * -------------------
 * --> module_init(function, type) => 
 * 					module_init(tmp123_register_types, MODULE_INIT_QOM)
 *                    |
 *                    V
 * -->  static void __attribute__((constructor)) do_qemu_init_ ## function =>
 * 					do_qemu_init_tmp123_register_types() =>
 * 						register_module_init(function, type)
 * 						  |
 * 						  V
 * 						register_module_init(tmp_register_types, MODULE_INIT_QOM)
 * 						{
 * 							ModuleEntry e;
 * 							e->init = tmp_register_types;
 * 							e->type = MODULE_INIT_QOM;
 * 						}
 */

static void tmp123_interrupt_update(TMP123State *s)
{
	/* 
	 * Alert pin will be low if pol = 0 (Active low) else Alert pin will be high
	 * if pol = 1 (Active high)
	 */
	qemu_set_irq(s->pin, s->alarm ^ ((~s->config >> 2) & 0x1));
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}

static void tmp123_alarm_update(TMP123State *s)
{
	if((s->config & 1) == 1) { /* SD mode */
		if((s->config >> 7) & 1 )  {/* One shot mode? */
			s->config &= ~(1<<7); /* Reset one-shot mode */
		} else {
			return;
		}
	}
	
	if((s->config >> 1) & 1) { /* Thermostat stat mode */
		/* Interrupt mode */
		if(s->temperature >= s->limit[1]) {
			s->alarm = 1;
		} else if(s->temperature <= s->limit[0]) {
			s->alarm = 1;
		}
	} else {
		/* Comparator mode */
		if(s->temperature >= s->limit[1]) {
			s->alarm = 1;
		} else if(s->temperature <= s->limit[0]) {
			s->alarm = 0;
		}
	}

	tmp123_interrupt_update(s);
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}

static void tmp123_set_temperature(
							Object *obj,
							Visitor *v,
							const char *name,
							void *opaque,
							Error **errp
						)
{
	TMP123State *s = TMP123(obj);
	Error *local_err = NULL;
	int64_t temp;

	visit_type_int(v, name, &temp, &local_err);

	if(local_err) {
		error_propagate(errp, local_err);
		return;
	}

	if(temp >= 128000 || temp < -128000) {
		error_setg(errp,
			"value %" PRId64 ".%03" PRIu64 " C is out of range", temp/1000, temp % 1000);
		return;
	}

	s->temperature = (int16_t) ((temp * 256)/1000);
	tmp123_alarm_update(s);
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}

static void tmp123_get_temperature(Object *obj,
								Visitor *v,
								const char *name,
								void *opaque,
								Error **errp)
{
	TMP123State *s = TMP123(obj);
	int64_t value = s->temperature * 1000 / 256;

	visit_type_int(v, name, &value, errp);
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}

static void tmp123_initfn(Object *obj)
{
	object_property_add(
						obj,
						"temperature",
						"int", 
						tmp123_get_temperature,
						tmp123_set_temperature,
						NULL,
						NULL,
						NULL
					);
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}


static void tmp123_reset(I2CSlave *i2c)
{
	TMP123State *s = TMP123(i2c);
	
	s->temperature = 0;
	s->pointer = 0;
	s->config = 0;
	s->faults = tmp123_faultq[(s->config >> 3) & 3];
	s->alarm = 0;

	tmp123_interrupt_update(s);
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}

static void tmp123_realize(DeviceState *dev, Error **errp)
{
	I2CSlave *i2c = I2C_SLAVE(dev);
	TMP123State *s = TMP123(i2c);

	qdev_init_gpio_out(&i2c->qdev, &s->pin, 1);

	tmp123_reset(&s->i2c);
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}


static void tmp123_write(TMP123State *s)
{
	switch(s->pointer & 3) {
		case TMP123_REG_TEMPERATURE:
			break;
		case TMP123_REG_CONFIG:
			/* 
			 * If buf[0] == 1 and if the existing SD config is 0
			 * then enable the tmp123 is going to shutdown
			 */
			if(s->buf[0] & ~s->config & (1 << 0)) {
				fprintf(stderr, "%s: TMP123 shutdown\n", __func__);
			}

			s->config = s->buf[0];
			s->faults = tmp123_faultq[(s->config >> 3) & 3];
			tmp123_alarm_update(s);
			break;
		case TMP123_REG_T_HIGH:
		case TMP123_REG_T_LOW:
			if(s->len >= 3) {
				s->limit[s->pointer & 1] = (int16_t) \
						((((uint16_t) s->buf[0]) << 8) | s->buf[1]);
			}
			break;
	}
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}

static int tmp123_tx(I2CSlave *i2c, uint8_t data)
{
	TMP123State *s = TMP123(i2c);
	
	/* 
	 * According to datasheet, First frame is Addr
	 * Second frame is pointer,
	 * Third and fourth frame is data.
	 */

	if(s->len == 0) {
		s->pointer = data;
		s->len++;
	} else {
		if(s->len <= 2) {
			s->buf[s->len - 1] = data;
		}
		s->len++;
		tmp123_write(s);
	}

	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
	return 0;
}

static uint8_t tmp123_rx(I2CSlave *i2c)
{
	TMP123State *s = TMP123(i2c);

	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);

	if(s->len < 2) {
		return s->buf[s->len++];
	}

	return 0xFF;
}

static void tmp123_read(TMP123State *s)
{
	s->len = 0;

	if((s->config >> 1) & 1) {
		s->alarm = 0;
		tmp123_interrupt_update(s);
	}

	switch(s->pointer & 3) {
		case TMP123_REG_TEMPERATURE:
			s->buf[s->len++] = (((uint16_t) s->temperature) >> 8);
			s->buf[s->len++] = (((uint16_t) s->temperature) >> 0) & \
									(0xF0 << ((~s->config >> 5) & 3));
			qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s %u %u \n", get_timestamp(),
									"TMP123_REG_TEMPERATURE",
									s->buf[0], s->buf[1]);
			break;
		case TMP123_REG_CONFIG:
			s->buf[s->len++] = s->config;
			qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), "TMP123_REG_CONFIG");
			break;
		case TMP123_REG_T_LOW:
			s->buf[s->len++] = ((uint16_t) s->limit[0]) >> 8;
			s->buf[s->len++] = ((uint16_t) s->limit[0]) >> 0;
			qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), "TMP123_REG_T_LOW");
			break;
		case TMP123_REG_T_HIGH:
			s->buf[s->len++] = ((uint16_t) s->limit[1]) >> 8;
			s->buf[s->len++] = ((uint16_t) s->limit[1]) >> 0;
			qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), "TMP123_REG_T_HIGH");
			break;
	}
	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
}

static int tmp123_event(I2CSlave *i2c, enum i2c_event event)
{
	TMP123State *s= TMP123(i2c);

	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);

	if(event == I2C_START_RECV) {
		qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), "I2C_START_RECV");
	} else if(event == I2C_START_SEND) {
		qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), "I2C_START_SEND");
	} else if(event == I2C_FINISH) {
		qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), "I2C_FINISH");
	} else if(event == I2C_NACK) {
		qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), "I2C_NACK");
	}

	if(event == I2C_START_RECV) {
		tmp123_read(s);
	}

	s->len = 0;
	/* All other events return 0 */
	return 0;
}

static int tmp123_post_load(void *opaque, int version_id)
{
	TMP123State *s = opaque;

	s->faults = tmp123_faultq[(s->config >> 3) & 3];

	tmp123_interrupt_update(s);

	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);
	return 0;
}

static const VMStateDescription vmstate_tmp123 = {
	.name = "TMP123",
	.version_id = 0,
	.minimum_version_id = 0,
	.post_load = tmp123_post_load,
	.fields = (VMStateField[]) {
		VMSTATE_UINT8(len, TMP123State),
		VMSTATE_UINT8_ARRAY(buf, TMP123State, 2),
		VMSTATE_UINT8(pointer, TMP123State),
		VMSTATE_UINT8(config, TMP123State),
		VMSTATE_INT16(temperature, TMP123State),
		VMSTATE_INT16_ARRAY(limit, TMP123State, 2),
		VMSTATE_UINT8(alarm, TMP123State),
		VMSTATE_I2C_SLAVE(i2c, TMP123State),
		VMSTATE_END_OF_LIST()
	}
};

static void tmp123_class_init(ObjectClass *klass, void *data)
{
	DeviceClass *dc = DEVICE_CLASS(klass);
	I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);

	qemu_log_mask(LOG_UNIMP, "[GAUN] %s %s \n", get_timestamp(), __func__);

	dc->realize = tmp123_realize;

	k->send = tmp123_tx;
	k->recv = tmp123_rx;
	k->event = tmp123_event;

	dc->vmsd = &vmstate_tmp123;
}

/*
 *  Object
 *    ^
 * 	  |
 *  DeviceState
 *    ^
 *    |
 *  I2CSlave
 *    ^
 *    |
 *  TMP123State
 */


static const TypeInfo tmp123_info = {
	.name = TYPE_TMP123,
	.parent = TYPE_I2C_SLAVE,
	.instance_size = sizeof(TMP123State),
	.instance_init = tmp123_initfn,
	.class_init = tmp123_class_init,
};

static void tmp123_register_types(void)
{
	type_register_static(&tmp123_info);
}

type_init(tmp123_register_types)
