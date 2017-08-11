package com.example.transonvu.vngbotbattle;

/**
 * Created by Tran Son Vu on 10/08/2017.
 */

public interface Constants {

    // Message types sent from the BluetoothService Handler
    public static final int MESSAGE_STATE_CHANGE = 1;
    public static final int MESSAGE_READ = 2;
    public static final int MESSAGE_WRITE = 3;
    public static final int MESSAGE_DEVICE_NAME = 4;
    public static final int MESSAGE_TOAST = 5;

    // Key names received from the BluetoothService Handler
    public static final String DEVICE_NAME = "device_name";
    public static final String TOAST = "toast";

    // Message sent for VBLUNO
    public static final String MESSAGE_STOP = "stop";
    public static final String MESSAGE_ATTACK = "attack";
    public static final String MESSAGE_CHANGE = "change";
}
