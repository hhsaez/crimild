/**
 * Crimild Engine is an open source scene graph based engine which purpose
 * is to fulfill the high-performance requirements of typical multi-platform
 * two and tridimensional multimedia projects, like games, simulations and
 * virtual reality.
 *
 * Copyright (C) 2006-2013 Hernan Saez - hhsaez@gmail.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <Crimild.hpp>

#include "gtest/gtest.h"

using namespace Crimild;

TEST( RectTest, testConstruction )
{
	Recti rect1( 10, 50, 100, 25 );
	EXPECT_TRUE( rect1.getX() == 10 );
	EXPECT_TRUE( rect1.getY() == 50 );
	EXPECT_TRUE( rect1.getWidth() == 100 );
	EXPECT_TRUE( rect1.getHeight() == 25 );

	Recti rect2( rect1 );
	EXPECT_TRUE( rect2.getX() == 10 );
	EXPECT_TRUE( rect2.getY() == 50 );
	EXPECT_TRUE( rect2.getWidth() == 100 );
	EXPECT_TRUE( rect2.getHeight() == 25 );

	rect2.x() = 20;
	EXPECT_TRUE( rect2.getX() == 20 );
}

