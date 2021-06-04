// SPDX-License-Identifier: GPL-2.0-only
/*
 * AK0991X 3-Axis Magnetometer (SPI)
 *
 * Copyright (C) 2014 ASAHI KASEI MICRODEVICES CORPORATION.
 *
 * Authors: Rikita Yamada <yamada.rj (at) om.asahi-kasei.co.jp>
 */

/*#define DEBUG*/
/*#define VERBOSE_DEBUG*/

#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/spi/spi.h>

#include "ak0991x_iio.h"

#define MAX_SPI_FREQ_HZ 2500000

static const struct regmap_config ak0991x_spi_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	 /* set 7th bit for read operation */
	.read_flag_mask = BIT(7),
};

/***** Probe/Remove function **************************************************/
static int ak0991x_spi_probe(struct spi_device *spi)
{
	const struct spi_device_id *id = spi_get_device_id(spi);
	struct regmap *regmap;

	/* don't exceed max specified SPI CLK frequency */
	if (spi->max_speed_hz > MAX_SPI_FREQ_HZ) {
		dev_err(&spi->dev, "spi CLK %d Hz too fast\n",
			spi->max_speed_hz);
		return -EINVAL;
	}

	regmap = devm_regmap_init_spi(spi, &ak0991x_spi_regmap_config);
	if (IS_ERR(regmap)) {
		dev_err(&spi->dev, "spi regmap initializing error: %ld\n",
			PTR_ERR(regmap));
		return PTR_ERR(regmap);
	}

	return ak0991x_probe(&spi->dev, regmap, spi->irq, id->name);
}

static int ak0991x_spi_remove(struct spi_device *spi)
{
	return ak0991x_remove(&spi->dev);
}

/***** SPI interface **********************************************************/
static const struct i2c_device_id ak0991x_spi_id[] = {
	{ AKM_DRIVER_NAME, 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, ak0991x_spi_id);

static const struct of_device_id ak0991x_of_match[] = {
	{ .compatible = "asahi-kasei,ak09915", },
	{ .compatible = "asahi-kasei,ak09915d", },
	{ .compatible = "asahi-kasei,ak09917d", },
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

static struct spi_driver ak0991x_spi_driver = {
	.probe		= ak0991x_spi_probe,
	.remove		= ak0991x_spi_remove,
	.id_table	= ak0991x_spi_id,
	.driver = {
		.name = AKM_DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table = ak0991x_of_match,
		.acpi_match_table = ACPI_PTR(ak0991x_acpi_match),
	},
};

module_spi_driver(ak0991x_spi_driver)

MODULE_AUTHOR("Asahi Kasei Microdevices Corp. <ssg@om.asahi-kasei.co.jp>");
MODULE_DESCRIPTION("AK0991X SPI magnetometer driver");
MODULE_LICENSE("GPL v2");
