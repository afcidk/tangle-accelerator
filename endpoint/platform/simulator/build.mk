# Copyright (C) 2019-2020 BiiLabs Co., Ltd. and Contributors
# All Rights Reserved.
# This is free software; you can redistribute it and/or modify it under the
# terms of the MIT license. A copy of the license can be found in the file
# "LICENSE" at the root of this distribution.

platform-build-command = \
	cd endpoint && mkapp -v -t localhost -C -DENABLE_ENDPOINT_TEST $(LEGATO_FLAGS) endpoint.adef;