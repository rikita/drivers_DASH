/* drivers/input/misc/ak8963-i2c.c - AK8963 compass driver
 *
 * Copyright (C) 2014 ASAHI KASEI MICRODEVICES CORPORATION.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*#define DEBUG*/
/*#define VERBOSE_DEBUG*/

#include <linux/ak8963.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/types.h>
#include "ak8963_input.h"

/***** I2C Tx/Rx operation ******************************************/
static int aki2c_rxdata(struct device *dev, unsigned char *rxdata,
						int length)
{
	struct i2c_client *i2c = to_i2c_client(dev);
	int ret;

	struct i2c_msg msgs[] = {
		{
			.addr = i2c->addr,
			.flags = 0,
			.len = 1,
			.buf = rxdata,
		},
		{
			.addr = i2c->addr,
			.flags = I2C_M_RD,
			.len = length,
			.buf = rxdata,
		},
	};

	ret = i2c_transfer(i2c->adapter, msgs, ARRAY_SIZE(msgs));
	if (ret < 0) {
		dev_err(dev, "%s: transfer failed.", __func__);
		return ret;
	} else if (ret != ARRAY_SIZE(msgs)) {
		dev_err(dev, "%s: transfer failed(size error).",
			__func__);
		return -ENXIO;
	}

	dev_vdbg(dev, "RxData: len=%02x, addr=%02x  data=%02x",
		length, rxdata[0], rxdata[1]);
	return 0;
}

static int aki2c_txdata(struct device *dev, unsigned char *txdata,
						int length)
{
	struct i2c_client *i2c = to_i2c_client(dev);
	int ret;

	struct i2c_msg msg[] = {
		{
			.addr = i2c->addr,
			.flags = 0,
			.len = length,
			.buf = txdata,
		},
	};

	ret = i2c_transfer(i2c->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0) {
		dev_err(dev, "%s: transfer failed.", __func__);
		return ret;
	} else if (ret != ARRAY_SIZE(msg)) {
		dev_err(dev, "%s: transfer failed(size error).",
			__func__);
		return -ENXIO;
	}

	dev_vdbg(dev, "TxData: len=%02x, addr=%02x data=%02x",
		length, txdata[0], txdata[1]);
	return 0;
}


static const struct ak8963_bus_ops ak8963_i2c_bops = {
	.bustype = BUS_I2C,
	.rxdata  = aki2c_rxdata,
	.txdata  = aki2c_txdata,
};

/***** Probe/Remove function ****************************************/
static int ak8963_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct ak8963_data *akm;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev,
			"%s: check_functionality failed.", __func__);
		return -ENODEV;
	}

	akm = ak8963_probe(&client->dev, client->irq, &ak8963_i2c_bops);
	if (IS_ERR(akm))
		return PTR_ERR(akm);

	/* Success */
	i2c_set_clientdata(client, akm);
	return 0;
}

static int ak8963_i2c_remove(struct i2c_client *client)
{
	struct ak8963_data *akm = i2c_get_clientdata(client);
	return ak8963_remove(akm);
}

/***** Power management *********************************************/
static int ak8963_i2c_suspend(struct device *dev)
{
	struct i2c_client *i2c = to_i2c_client(dev);
	struct ak8963_data *akm = i2c_get_clientdata(i2c);

	return ak8963_suspend(akm);
}

static int ak8963_i2c_resume(struct device *dev)
{
	struct i2c_client *i2c = to_i2c_client(dev);
	struct ak8963_data *akm = i2c_get_clientdata(i2c);

	return ak8963_resume(akm);
}

static const struct dev_pm_ops ak8963_i2c_pops = {
	.suspend	= ak8963_i2c_suspend,
	.resume		= ak8963_i2c_resume,
};

/***** I2C interface ***********************************************/
static const struct i2c_device_id ak8963_id[] = {
	{AKM_DRIVER_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ak8963_id);

static struct i2c_driver ak8963_i2c_driver = {
	.probe		= ak8963_i2c_probe,
	.remove		= ak8963_i2c_remove,
	.id_table	= ak8963_id,
	.driver = {
		.name = AKM_DRIVER_NAME,
		.owner = THIS_MODULE,
		.pm = &ak8963_i2c_pops,
	},
};

static int __init ak8963_i2c_init(void)
{
	return i2c_add_driver(&ak8963_i2c_driver);
}

static void __exit ak8963_i2c_exit(void)
{
	i2c_del_driver(&ak8963_i2c_driver);
}

module_init(ak8963_i2c_init);
module_exit(ak8963_i2c_exit);

MODULE_AUTHOR("Asahi Kasei Microdevices Corp. <multi-s@om.asahi-kasei.co.jp>");
MODULE_DESCRIPTION("AK8963 I2C compass driver");
MODULE_LICENSE("GPL");
