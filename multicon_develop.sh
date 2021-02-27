build_package()
{
    rm -rf /home/biagioscarciello/Scrivania/RUT9XX_R_GPL_00.06.07.3/build_dir/target-mips_34kc_uClibc-0.9.33.2/multicon-1.0
    make package/multicon/compile V=s
}

upgrade_device()
{
    echo "[MULTICON-UPDATER] Start updating process..."
    echo "[MULTICON-UPDATER] Removing old package..."
    sshpass -p Maradona10 ssh -q -p 2222 root@amendola.no-ip.biz << EOF
        cd /root/; 
        rm multicon*
EOF
    echo "[MULTICON-UPDATER] Copyng new package..."
    sshpass -p Maradona10 scp -P 2222 bin/ar71xx/packages/multicon_1.0-1_ar71xx.ipk root@amendola.no-ip.biz:/root 
    echo "[MULTICON-UPDATER] Uninstall old package and install new package:"
    sshpass -p Maradona10 ssh -q -p 2222 root@amendola.no-ip.biz << EOF
    opkg remove multicon; 
    opkg install multicon_1.0-1_ar71xx.ipk
EOF
    echo "[MULTICON-UPDATER] Set app starting on boot..."
    sshpass -p Maradona10 scp -P 2222 multicon_utils/multicon root@amendola.no-ip.biz:/etc/init.d 
    sshpass -p Maradona10 ssh -q -p 2222 root@amendola.no-ip.biz << EOF 
        /etc/init.d/multicon enable
EOF
    echo "[MULTICON-UPDATER] Cleaning directories..."
    sshpass -p Maradona10 ssh -q -p 2222 root@amendola.no-ip.biz << EOF 
        cd /
        rm *.csv
EOF
}

case "$1" in
    'build')
    build_package
    ;;
    'upgrade')
    upgrade_device
    ;;
    'build-upgrade')
    build_package
    upgrade_device
    ;;
    'run')
    cd multicon/
    make -f MakefileDevelop run
    ;;
    'clean-workspace')
    cd multicon/
    make -f MakefileDevelop clean
esac
