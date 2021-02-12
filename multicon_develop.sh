build_package()
{
    rm -rf /home/biagioscarciello/Scrivania/RUT9XX_R_GPL_00.06.07.3/build_dir/target-mips_34kc_uClibc-0.9.33.2/multicon-1.0
    make package/multicon/compile V=s
}
upgrade_device()
{
    echo "[MULTICON-UPDATER] Start updating process..."
    sshpass -p Maradona10 ssh -p 2222 root@10.147.18.225 "cd /root/"
    echo "[MULTICON-UPDATER] Removing old package..."
    sshpass -p Maradona10 ssh -p 2222 root@10.147.18.225 "rm multicon*"
    echo "[MULTICON-UPDATER] Copyng new package..."
    cd bin/ar71xx/packages/
    sshpass -p Maradona10 scp -P 2222 multicon_1.0-1_ar71xx.ipk root@10.147.18.225:/root 
    echo "[MULTICON-UPDATER] Uninstall old package..."
    sshpass -p Maradona10 ssh -p 2222 root@10.147.18.225 "opkg remove multicon"
    echo "[MULTICON-UPDATER] Install new package:"
    sshpass -p Maradona10 ssh -p 2222 root@10.147.18.225  "opkg install multicon_1.0-1_ar71xx.ipk"
}
upgrade_device_1()
{
    echo "[MULTICON-UPDATER] Start updating process..."
    echo "[MULTICON-UPDATER] Removing old package..."
    sshpass -p Maradona10 ssh -q -p 2222 root@10.147.18.225 << EOF
        cd /root/; 
        rm multicon*
EOF
    echo "[MULTICON-UPDATER] Copyng new package..."
    cd bin/ar71xx/packages/
    sshpass -p Maradona10 scp -P 2222 multicon_1.0-1_ar71xx.ipk root@10.147.18.225:/root 
    echo "[MULTICON-UPDATER] Uninstall old package and install new package:"
    sshpass -p Maradona10 ssh -q -p 2222 root@10.147.18.225 << EOF
        opkg remove multicon; 
        opkg install multicon_1.0-1_ar71xx.ipk
EOF
}

case "$1" in
    'build')
    build_package
    ;;
    'upgrade')
    upgrade_device_1
    ;;
    'build-upgrade')
    build_package
    upgrade_device_1
    ;;
    'run')
    cd multicon/
    make -f MakefileDevelop run
    ;;
esac
