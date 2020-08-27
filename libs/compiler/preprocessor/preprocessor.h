/*
 *	Copyright 2018 Andrey Terekhov, Egor Anikin
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 */

#pragma once

#include "compiler.h"
#include "context.h"
#include "context_var.h"


#ifdef __cplusplus
extern "C" {
#endif

char *preprocess_file(int argc, const char *argv[], data_files *sources, data_files *headers);
void preprocess_scan(preprocess_context *context);

#ifdef __cplusplus
} /* extern "C" */
#endif