import os
import sys
import subprocess
from waflib import Logs, Errors

def get_rtgl1_sdk(ctx):
	sdk_path = getattr(ctx.env, 'RTGL1_SDK_PATH', None) or os.environ.get('RTGL1_SDK_PATH', '')
	if sdk_path and os.path.isdir(sdk_path):
		return os.path.abspath(sdk_path)

	sibling = os.path.abspath(os.path.join(ctx.srcnode.abspath(), '..', 'RayTracedGL1'))
	if os.path.isdir(sibling):
		return sibling

	local_sub = os.path.abspath(os.path.join(ctx.srcnode.abspath(), 'rtgl1_sdk'))
	if os.path.isdir(local_sub):
		return local_sub

	Logs.info('RTGL1 SDK not found. Cloning https://github.com/boofiboi/RayTracedGL1...')
	cmd = ['git', 'clone', '--recursive', 'https://github.com/boofiboi/RayTracedGL1', '-b', 'hl1', local_sub]
	ret = subprocess.call(cmd)
	if ret != 0 or not os.path.isdir(local_sub):
		raise Errors.WafError('Failed to clone RayTracedGL1 repository')

	return local_sub

def ensure_rtgl1_built(ctx, sdk_path):
	candidates = [
		os.path.join(sdk_path, 'Build', 'x64-Release', 'Release', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'x64-Release', 'RelWithDebInfo', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'x64-Release', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'Release', 'Release', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'Release', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'x64-Debug', 'Debug', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'x64-Debug', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'Debug', 'Debug', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'Debug', 'RayTracedGL1.lib'),
		os.path.join(sdk_path, 'Build', 'x64-Release-NonAVX', 'Release', 'RayTracedGL1.lib'),
	]
	for c in candidates:
		if os.path.exists(c):
			return

	Logs.info('RayTracedGL1.lib not found. Building RayTracedGL1 via CMake...')
	shaders_dir = os.path.join(sdk_path, 'Source', 'Shaders')
	if os.path.isdir(shaders_dir):
		ret = subprocess.call([sys.executable, 'GenerateShaders.py'], cwd=shaders_dir)
		if ret != 0:
			Logs.warn('Shader generation exited with code %d' % ret)

	cmake_configure = [
		'cmake', '-B', 'Build/x64-Release',
		'-DCMAKE_BUILD_TYPE=Release',
		'-DRG_WITH_SURFACE_WIN32=ON',
		'-DRG_WITH_AMD_FSR3=ON',
		'-DRG_WITH_NVIDIA_NRD=ON',
		'-DRG_WITH_SHADERS=OFF',
		'-DRG_WITH_EXAMPLES=OFF',
		'-DCMAKE_CXX_FLAGS_RELEASE=/O2 /Ob3 /Ot /Oi /GL /Gw /Gy /fp:fast /arch:AVX2 /DNDEBUG'
	]
	ret = subprocess.call(cmake_configure, cwd=sdk_path)
	if ret != 0:
		raise Errors.WafError('CMake configure failed for RayTracedGL1')

	cmake_build = ['cmake', '--build', 'Build/x64-Release', '--config', 'Release', '--parallel']
	ret = subprocess.call(cmake_build, cwd=sdk_path)
	if ret != 0:
		raise Errors.WafError('CMake build failed for RayTracedGL1')

def configure(conf):
	if conf.env.RAYTRACING:
		sdk_path = get_rtgl1_sdk(conf)
		ensure_rtgl1_built(conf, sdk_path)
		conf.env.RTGL1_SDK_PATH = sdk_path
