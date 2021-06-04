// SPDX-License-Identifier: GPL-2.0-only
/*
 * AK0991X 3-Axis Magnetometer
 *
 * Copyright (C) 2014 ASAHI KASEI MICRODEVICES CORPORATION.
 *
 * Authors: Rikita Yamada <yamada.rj (at) om.asahi-kasei.co.jp>
 */
#ifndef _AK0991X_IIO_H_
#define _AK0991X_IIO_H_

#define AKM_DRIVER_NAME "ak0991x"

struct device;
struct ak0991x_data;

struct ak0991x_bus_ops {
	int (*rxdata)(struct device *dev, unsigned char *rxdata, int length);
	int (*txdata)(struct device *dev, unsigned char *txdata, int length);
};

int ak0991x_suspend(struct ak0991x_data *akm);
int ak0991x_resume(struct ak0991x_data *akm);
int ak0991x_probe(struct device *dev, struct regmap *regmap, int irq,
		  const char *name);
int ak0991x_remove(struct device *dev);

#endif
