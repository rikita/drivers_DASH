// SPDX-License-Identifier: GPL-2.0-only
/*
 * AK0991X 3-Axis Magnetometer (I2C)
 *
 * Copyright (C) 2014 ASAHI KASEI MICRODEVICES CORPORATION.
 *
 * Authors: Rikita Yamada <yamada.rj (at) om.asahi-kasei.co.jp>
 */

/*#define DEBUG*/
/*#define VERBOSE_DEBUG*/

#include <linux/acpi.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include "ak0991x_iio.h"

static const struct regmap_config ak0991x_i2c_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
};

/***** Probe/Remove function **************************************************/
static int ak0991x_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct regmap *regmap;

	if (!id)
		return -ENODEV;

	regmap = devm_regmap_init_i2c(client, &ak0991x_i2c_regmap_config);
	if (IS_ERR(regmap)) {
		dev_err(&client->dev, "i2c regmap initializing error: %ld\n",
			PTR_ERR(regmap));
		return PTR_ERR(regmap);
	}

	return ak0991x_probe(&client->dev, regmap, client->irq, id->name);
}

static int ak0991x_i2c_remove(struct i2c_client *client)
{
	return ak0991x_remove(&client->dev);
}

/***** I2C interface **********************************************************/
static const struct i2c_device_id ak0991x_i2c_id[] = {
	{ AKM_DRIVER_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, ak0991x_i2c_id);

static const struct of_device_id ak0991x_of_match[] = {
	{ .compatible = "asahi-kasei,ak09915", },
	{ .compatible = "asahi-kasei,ak09915d", },
	{ .compatible = "asahi-kasei,ak09916", },
	{ .compatible = "asahi-kasei,ak09916d", },
	{ .compatible = "asahi-kasei,ak09917d", },
	{ .compatible = "asahi-kasei,ak09918", },
	{ .compatible = "asahi-kasei,ak09919", },
	{ },
};
MODULE_DEVICE_TABLE(of, ak0991x_of_match);

#ifdef CONFIG_ACPI
static const struct acpi_device_id ak0991x_acpi_match[] = {
	{ AKM_DRIVER_NAME, 0 },
	{ },
};
MODULE_DEVICE_TABLE(acpi, ak0991x_acpi_match);
#endif

static struct i2c_driver ak0991x_i2c_driver = {
	.probe		= ak0991x_i2c_probe,
	.remove		= ak0991x_i2c_remove,
	.id_table	= ak0991x_i2c_id,
	.driver = {
		.name = AKM_DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = ak0991x_of_match,
		.acpi_match_table = ACPI_PTR(ak0991x_acpi_match),
	},
};

module_i2c_driver(ak0991x_i2c_driver)

MODULE_AUTHOR("Asahi Kasei Microdevices Corp. <ssg@om.asahi-kasei.co.jp>");
MODULE_DESCRIPTION("AK0991X I2C magnetometer driver");
MODULE_LICENSE("GPL v2");
