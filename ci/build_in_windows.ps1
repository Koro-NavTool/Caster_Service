
cmake -B build -DCMAKE_CXX_COMPILER=cl -DCMAKE_C_COMPILER=cl -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=bin 

cmake --build build --config Release -- /m

# �ƶ������ļ�����Ŀ¼�� build/release �� build
Move-Item -Path "bin\Release\*" -Destination "bin"  -Force

# ɾ�� build/release Ŀ¼
Remove-Item -Path "bin\Release" -Recurse -Force

# ����Ŀ��Ŀ¼
New-Item -Path "bin\env" -ItemType Directory -Force
# ����Ŀ��Ŀ¼
New-Item -Path "bin\env\redis" -ItemType Directory -Force

# �ƶ�Ŀ¼���ļ�
Copy-Item -Path "env\Redis-7.4.0-Windows-x64-msys2\*" -Destination "bin\env\redis" -Recurse -Force

# �����ļ�����������
Copy-Item -Path ".cmake\redis_windows.conf.in" -Destination "bin\env\redis\redis.conf" -Force

Copy-Item -Path "env\scripts" -Destination "bin\env\scripts" -Recurse -Force