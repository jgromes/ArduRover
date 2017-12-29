package com.example.atlas.myapplication;

import com.physicaloid.lib.Physicaloid;
import com.physicaloid.lib.usb.driver.uart.ReadLisener;

import android.bluetooth.BluetoothSocket;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;

import android.graphics.drawable.Drawable;
import android.support.v7.app.AppCompatActivity;
import android.view.*;

import android.os.Bundle;
import android.os.Handler;

import android.text.Editable;
import android.text.Html;
import android.text.TextWatcher;

import android.content.res.Resources;
import android.content.res.Resources.Theme;

import android.widget.AdapterView;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.Spinner;
import android.widget.TabHost;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.ScrollView;

import java.io.IOException;
import java.util.Set;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class MainActivity extends AppCompatActivity {
    Physicaloid port;

    BluetoothAdapter btAdapter;
    private final UUID PORT_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb");
    private BluetoothDevice device;
    private BluetoothSocket socket;
    private OutputStream outputStream;
    private InputStream inputStream;
    Thread thread;
    byte buffer[];
    int bufferPosition;
    boolean stopThread;

    TextView text_port_status, text_monitor_output, text_monitor_input, text_camera_pan, text_camera_tilt;
    TextView text_acceleration_x, text_acceleration_y, text_acceleration_z, text_temperature, text_pressure, text_current_left;
    TextView text_magnetic_induction_x, text_magnetic_induction_y, text_magnetic_induction_z, text_uv_radiation, text_ambient_light, text_current_right;
    Spinner spinner_baud, spinner_line, spinner_connection, spinner_lora_bandwidth, spinner_lora_spreading_factor, spinner_lora_coding_rate;
    CheckBox checkbox_autoscroll;
    ScrollView monitor_output_scroll;

    boolean portConnected, btConnected;
    String lineEnding = "\n";
    boolean autoscroll = true;

    int camera_pan = 85;
    int camera_tilt = 130;

    public final int CMD_MOTOR_LR_STOP = 0x00;
    public final int CMD_MOTOR_LR_SPEED = 0x10;
    public final int CMD_MOTOR_L_SPEED = 0x20;
    public final int CMD_MOTOR_R_SPEED = 0x30;
    public final int CMD_CAMERA_TILT = 0x40;
    public final int CMD_CAMERA_PAN = 0x50;
    public final int CMD_CAMERA_SNAP = 0x60;
    public final int CMD_CAMERA_SEND = 0x70;
    public final int CMD_SENSOR_MEASURE = 0x80;
    public final int CMD_SENSOR_SEND = 0x90;
    public final int CMD_LORA_RESEND_LAST = 0xA0;
    public final int CMD_LORA_CONFIG = 0xB0;
    public final int CMD_LORA_REQUEST_ERRORS = 0xC0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Resources res = getResources();
        Theme th = getTheme();
        port = new Physicaloid(this);
        portConnected = false;

        btAdapter = BluetoothAdapter.getDefaultAdapter();
        btConnected = false;

        text_port_status = findViewById(R.id.text_port_status);
        text_port_status.setText(R.string.port_status_closed);

        spinner_baud = findViewById(R.id.spinner_baudrate);
        spinner_baud.setSelection(4);

        spinner_line = findViewById(R.id.spinner_line);
        spinner_line.setSelection(1);

        spinner_connection = findViewById(R.id.spinner_connection);
        spinner_connection.setSelection(1);

        spinner_lora_bandwidth = findViewById(R.id.spinner_lora_bandwidth);
        spinner_lora_bandwidth.setSelection(7);

        spinner_lora_spreading_factor = findViewById(R.id.spinner_lora_spreading_factor);
        spinner_lora_spreading_factor.setSelection(3);

        spinner_lora_coding_rate = findViewById(R.id.spinner_lora_coding_rate);
        spinner_lora_coding_rate.setSelection(2);

        checkbox_autoscroll = findViewById(R.id.checkBox_auto_scroll);

        text_monitor_output = findViewById(R.id.monitor_output);
        monitor_output_scroll = findViewById(R.id.monitor_output_scroll);
        text_monitor_output.addTextChangedListener(tw_autoscroll);

        text_monitor_input = findViewById(R.id.monitor_input);

        text_camera_pan = findViewById(R.id.text_camera_pan);
        text_camera_tilt = findViewById(R.id.text_camera_tilt);

        TabHost host = findViewById(R.id.tabHost);
        host.setup();

        Drawable icon = res.getDrawable(R.drawable.icon_camera, th);
        TabHost.TabSpec spec = host.newTabSpec("Camera");
        spec.setContent(R.id.tab_camera);
        spec.setIndicator("", icon);
        host.addTab(spec);

        icon = res.getDrawable(R.drawable.icon_gps, th);
        spec = host.newTabSpec("GPS");
        spec.setContent(R.id.tab_gps);
        spec.setIndicator("", icon);
        host.addTab(spec);

        icon = res.getDrawable(R.drawable.icon_graphs, th);
        spec = host.newTabSpec("Graphs");
        spec.setContent(R.id.tab_graphs);
        spec.setIndicator("", icon);
        host.addTab(spec);

        icon = res.getDrawable(R.drawable.icon_sensors, th);
        spec = host.newTabSpec("Sensors");
        spec.setContent(R.id.tab_sensors);
        spec.setIndicator("", icon);
        host.addTab(spec);

        icon = res.getDrawable(R.drawable.icon_monitor, th);
        spec = host.newTabSpec("Monitor");
        spec.setContent(R.id.tab_monitor);
        spec.setIndicator("", icon);
        host.addTab(spec);

        icon = res.getDrawable(R.drawable.icon_settings, th);
        spec = host.newTabSpec("Settings");
        spec.setContent(R.id.tab_settings);
        spec.setIndicator("", icon);
        host.addTab(spec);

        host.setCurrentTab(5);

        final SeekBar seekbar_motor_left = findViewById(R.id.motor_speed_left);
        seekbar_motor_left.setProgress(50);
        final SeekBar seekbar_motor_right = findViewById(R.id.motor_speed_right);
        seekbar_motor_right.setProgress(50);

        //Stop button
        final Button button_stop = findViewById(R.id.button_stop);
        button_stop.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                seekbar_motor_left.setProgress(50);
                seekbar_motor_right.setProgress(50);
                writeToPort(CMD_MOTOR_LR_STOP);
            }
        });

        //Left side motors
        seekbar_motor_left.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                int direction = 0;
                int level = seekBar.getProgress() - 50;
                if(level < 0) {
                    direction = 1;
                    level = -1 * level;
                }
                level = level * 5;
                writeToPort(CMD_MOTOR_L_SPEED, level, direction);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        //Right side motors
        seekbar_motor_right.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                int direction = 0;
                int level = seekBar.getProgress() - 50;
                if(level < 0) {
                    direction = 1;
                    level = -1 * level;
                }
                level = level * 5;
                writeToPort(CMD_MOTOR_R_SPEED, level, direction);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });


        //Camera up button
        final Button button_camera_up = findViewById(R.id.btn_camera_up);
        button_camera_up.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if((camera_tilt >= 15) && (portConnected || btConnected)) {
                    camera_tilt -= 5;
                    text_camera_tilt.setText(Integer.toString(camera_tilt));
                    writeToPort(CMD_CAMERA_TILT, camera_tilt);
                }

            }
        });

        //Camera down button
        final Button button_camera_down = findViewById(R.id.btn_camera_down);
        button_camera_down.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if((camera_tilt <= 125) && (portConnected || btConnected)) {
                    camera_tilt += 5;
                    text_camera_tilt.setText(Integer.toString(camera_tilt));
                    writeToPort(CMD_CAMERA_TILT, camera_tilt);
                }

            }
        });

        //Camera snap button
        final Button button_camera_snap = findViewById(R.id.btn_camera_snap);
        button_camera_snap.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                writeToPort(CMD_CAMERA_SNAP);
            }
        });

        //Camera right button
        final Button button_camera_right = findViewById(R.id.btn_camera_right);
        button_camera_right.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if((camera_pan >= 15) && (portConnected || btConnected)) {
                    camera_pan -= 5;
                    text_camera_pan.setText(Integer.toString(camera_pan));
                    writeToPort(CMD_CAMERA_PAN, camera_pan);
                }

            }
        });

        //Camera left button
        final Button button_camera_left = findViewById(R.id.btn_camera_left);
        button_camera_left.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if((camera_pan <= 165) && (portConnected || btConnected)) {
                    camera_pan += 5;
                    text_camera_pan.setText(Integer.toString(camera_pan));
                    writeToPort(CMD_CAMERA_PAN, camera_pan);
                }

            }
        });

        //Camera save button
        final Button button_camera_save = findViewById(R.id.btn_camera_save);
        button_camera_save.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                writeToPort(CMD_CAMERA_SEND, 0x00);
            }
        });

        //Sensors measure button
        final Button button_sensors_measure = findViewById(R.id.sensors_force);
        button_sensors_measure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                writeToPort(CMD_SENSOR_MEASURE, 0xFF);
            }
        });

        //Sensors measure all button
        final Button button_sensors_measure_all = findViewById(R.id.sensors_update);
        button_sensors_measure_all.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                writeToPort(CMD_SENSOR_SEND, 0xFF);
            }
        });

        //Monitor Send button
        final Button button_monitor_send = findViewById(R.id.monitor_send);
        button_monitor_send.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(portConnected) {
                    String input = text_monitor_input.getText().toString();
                    text_monitor_output.append("\n");
                    text_monitor_output.append(input);
                    text_monitor_output.append(lineEnding);
                    port.write(input.getBytes());
                }
            }
        });

        //Port line ending spinner
        spinner_line.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                String line_string = spinner_line.getSelectedItem().toString();
                switch(line_string) {
                    case "None":
                        lineEnding = "";
                        break;
                    case "New Line":
                        lineEnding = "\n";
                        break;
                    case "Carriage Return":
                        lineEnding = "\r";
                        break;
                    case "Both":
                        lineEnding = "\r\n";
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
                lineEnding = "\n";
            }
        });

        //LoRa bandwidth spinner
        //LoRa spreading factor spinner
        //LoRa coding rate spinner

        //Request errors button
        final Button button_request_errors = findViewById(R.id.button_request_errors);
        button_request_errors.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                writeToPort(CMD_LORA_REQUEST_ERRORS);
            }
        });

        //Recofigure LoRa button
        final Button button_lora_reconfigure = findViewById(R.id.button_lora_reconfigure);
        button_lora_reconfigure.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                writeToPort(CMD_LORA_CONFIG, spinner_lora_bandwidth.getSelectedItemPosition(), spinner_lora_spreading_factor.getSelectedItemPosition(), spinner_lora_coding_rate.getSelectedItemPosition());
            }
        });

        //Port autoscroll
        checkbox_autoscroll.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(!autoscroll) {
                    text_monitor_output.addTextChangedListener(tw_autoscroll);
                    autoscroll = true;
                } else {
                    text_monitor_output.removeTextChangedListener(tw_autoscroll);
                    autoscroll = false;
                }
            }
        });

        //Open port button
        final Button button_port_open = findViewById(R.id.button_port_open);
        button_port_open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                switch (spinner_connection.getSelectedItem().toString()) {
                    case "USB port":
                        if(!portConnected) {
                            try {
                                String baud_string = spinner_baud.getSelectedItem().toString();
                                port.setBaudrate(Integer.parseInt(baud_string));

                                if (port.open()) {
                                    portConnected = true;
                                    text_monitor_output.append("Port opened\n");
                                    text_port_status.setText(R.string.port_status_open);

                                    port.addReadListener(new ReadLisener() {
                                        @Override
                                        public void onRead(int i) {
                                            byte[] buf = new byte[i];
                                            port.read(buf, i);
                                            parseResponse(buf);
                                            tvAppend(text_monitor_output, Html.fromHtml("<font color=blue>" + bytesToHex(buf) + "</font>"));
                                        }
                                    });
                                }
                            } catch (Exception ex) {
                                portConnected = false;
                                text_monitor_output.append("\n");
                                text_monitor_output.append(Html.fromHtml("<font color=red>" + ex.getMessage() + "</font>"));
                                text_monitor_output.append("\n");
                            }
                        }
                        break;
                    case "Bluetooth":
                        if(!btConnected) {
                            try {
                                if (!btAdapter.isEnabled()) {
                                    Intent enableAdapter = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                                    startActivityForResult(enableAdapter, 0);
                                }
                                Set<BluetoothDevice> bondedDevices = btAdapter.getBondedDevices();
                                for (BluetoothDevice iterator : bondedDevices) {
                                    if (iterator.getName().equals("HC-05")) {
                                        device = iterator;
                                        break;
                                    }
                                }
                                socket = device.createRfcommSocketToServiceRecord(PORT_UUID);
                                socket.connect();
                                btConnected = true;
                                text_monitor_output.append("BT connected\n");
                                text_port_status.setText(R.string.port_status_open);
                                inputStream = socket.getInputStream();
                                outputStream = socket.getOutputStream();
                                beginListenForData();
                            } catch (Exception ex) {
                                btConnected = false;
                                text_monitor_output.append("\n");
                                text_monitor_output.append(Html.fromHtml("<font color=red>" + ex.getMessage() + "</font>"));
                                text_monitor_output.append("\n");
                            }
                        }
                        break;
                }
            }
        });

        //Close port button
        final Button button_port_close = findViewById(R.id.button_port_close);
        button_port_close.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                switch (spinner_connection.getSelectedItem().toString()) {
                    case "USB port":
                        if(portConnected) {
                            port.close();
                            portConnected = false;
                            text_port_status.setText(R.string.port_status_closed);
                            text_monitor_output.append("\nPort closed\n");
                        }
                        break;
                    case "Bluetooth":
                        if(btConnected) {
                            try {
                                socket.close();
                                btConnected = false;
                                text_port_status.setText(R.string.port_status_closed);
                                text_monitor_output.append("\nBT disconnected\n");
                            } catch (Exception ex) {
                                btConnected = false;
                                text_monitor_output.append("\n");
                                text_monitor_output.append(Html.fromHtml("<font color=red>" + ex.getMessage() + "</font>"));
                                text_monitor_output.append("\n");
                            }
                        }
                        break;
                }
            }
        });
    }

    TextWatcher tw_autoscroll = new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
            monitor_output_scroll.fullScroll(ScrollView.FOCUS_DOWN);
        }

        @Override
        public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {

        }

        @Override
        public void afterTextChanged(Editable editable) {

        }
    };

    Handler mHandler = new Handler();
    private void tvAppend(TextView tv, CharSequence text) {
        final TextView ftv = tv;
        final CharSequence ftext = text;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                ftv.append(ftext);
            }
        });
    }

    private void writeToPort(int cmd0) {
        byte cmd[] = {(byte)cmd0, 0x00, 0x00, 0x00};
        writeToPort(cmd);
    }

    private void writeToPort(int cmd0, int cmd1) {
        byte cmd[] = {(byte)cmd0, (byte)cmd1, 0x00, 0x00};
        writeToPort(cmd);
    }

    private void writeToPort(int cmd0, int cmd1, int cmd2) {
        byte cmd[] = {(byte)cmd0, (byte)cmd1, (byte)cmd2, 0x00};
        writeToPort(cmd);
    }

    private void writeToPort(int cmd0, int cmd1, int cmd2, int cmd3) {
        byte cmd[] = {(byte)cmd0, (byte)cmd1, (byte)cmd2, (byte)cmd3};
        writeToPort(cmd);
    }

    private void writeToPort(byte[] cmd) {
        switch (spinner_connection.getSelectedItem().toString()) {
            case "USB port":
                if(portConnected) {
                    port.write(cmd);
                    text_monitor_output.append("\n");
                    text_monitor_output.append(Html.fromHtml("<font color=black>" + bytesToHex(cmd) +"</font>"));
                    text_monitor_output.append("\n");
                }
                break;
            case "Bluetooth":
                if(btConnected) {
                    try {
                        outputStream.write(cmd);
                        /*for(int i = 0; i < 4; i++) {
                            int cmdInt = cmd[i] & 0xFF;
                            outputStream.write(cmdInt);
                            text_monitor_output.append(Integer.toString(cmdInt));
                        }*/
                        text_monitor_output.append("\n");
                        text_monitor_output.append(Html.fromHtml("<font color=black>" + bytesToHex(cmd) + "</font>"));
                        text_monitor_output.append("\n");
                        //long start = System.currentTimeMillis();
                        //while((inputStream.available() == 0) && (System.currentTimeMillis() - start < 5000));
                    } catch (Exception ex) {
                        text_monitor_output.append("\n");
                        text_monitor_output.append(Html.fromHtml("<font color=red>" + ex.getMessage() + "</font>"));
                        text_monitor_output.append("\n");
                    }
                }
                break;
        }

    }

    private final static char[] hexArray = "0123456789ABCDEF".toCharArray();
    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }

    void beginListenForData() {
        final Handler handler = new Handler();
        stopThread = false;
        buffer = new byte[1024];

        Thread thread  = new Thread(new Runnable() {
            public void run() {
                while(!Thread.currentThread().isInterrupted() && !stopThread) {
                    try {
                        int byteCount = inputStream.available();
                        /*if(byteCount == 46) {
                            final byte[] sensorBytes = new byte[byteCount];
                            inputStream.read(sensorBytes);
                            handler.post(new Runnable() {
                                public void run() {
                                    text_monitor_output.append(Html.fromHtml("<font color=blue>" + bytesToHex(sensorBytes) + "</font>"));
                                    parseResponse(sensorBytes);
                                }
                            });
                        }*/
                        if(byteCount == 4) {
                            final byte[] rawBytes = new byte[byteCount];
                            inputStream.read(rawBytes);
                            handler.post(new Runnable() {
                                public void run() {
                                    text_monitor_output.append(Html.fromHtml("<font color=blue>" + bytesToHex(rawBytes) + "</font>"));
                                    parseResponse(rawBytes);
                                }
                            });

                        }
                    }
                    catch (IOException ex) {
                        stopThread = true;
                    }
                }
            }
        });

        thread.start();
    }

    void parseResponse(byte[] response) {
        if((response.length != 4) && (response.length != 46)) {
            text_monitor_output.append("\n");
            text_monitor_output.append(Html.fromHtml("<font color=red>Unable to parse response: " + response.length + " bytes received</font>"));
            return;
        }

        String respString;

        switch(response[0] & 0xF0) {
            case (byte)CMD_MOTOR_LR_STOP:
                respString = "CMD_MOTOR_LR_STOP - OK";
                break;
            case (byte)CMD_MOTOR_LR_SPEED:
                respString = "CMD_MOTOR_LR_SPEED - OK";
                break;
            case (byte)CMD_MOTOR_L_SPEED:
                respString = "CMD_MOTOR_L_SPEED - OK";
                break;
            case (byte)CMD_MOTOR_R_SPEED:
                respString = "CMD_MOTOR_R_SPEED - OK";
                break;
            case (byte)CMD_CAMERA_TILT:
                respString = "CMD_CAMERA_TILT - OK";
                break;
            case (byte)CMD_CAMERA_PAN:
                respString = "CMD_CAMERA_PAN - OK";
                break;
            case (byte)CMD_CAMERA_SNAP:
                switch(response[0] & 0x0F) {
                    case 0x00:
                        respString = "CMD_CAMERA_SNAP - OK";
                        break;
                    case 0x01:
                        respString = "CMD_CAMERA_SNAP - FAIL camera not found";
                        break;
                    case 0x02:
                        respString = "CMD_CAMERA_SNAP - FAIL unable to take picture";
                        break;
                    case 0x03:
                        respString = "CMD_CAMERA_SNAP - FAIL unable to save picture";
                        break;
                    default:
                        respString = "CMD_CAMERA_SNAP - FAIL unknown 0x" + String.format("%02X", response[0] & 0x0F);
                        break;
                }
                break;
            case (byte)CMD_CAMERA_SEND:
                switch(response[0] & 0x0F) {
                    case 0x00:
                        respString = "CMD_CAMERA_SEND - OK sending image info";
                        break;
                    case 0x01:
                        respString = "CMD_CAMERA_SEND - OK sending image data";
                        break;
                    case 0x02:
                        respString = "CMD_CAMERA_SEND - FAIL SD card not found";
                        break;
                    case 0x03:
                        respString = "CMD_CAMERA_SEND - FAIL image not found";
                        break;
                    default:
                        respString = "CMD_CAMERA_SEND - FAIL unknown 0x" + String.format("%02X", response[0] & 0x0F);
                        break;
                }
                break;
            case (byte)CMD_SENSOR_MEASURE:
                switch(response[0] & 0x0F) {
                    case 0x00:
                        respString = "CMD_SENSOR_MEASURE - OK";
                        break;
                    case 0x01:
                        respString = "CMD_SENSOR_MEASURE - FAIL";
                        if((response[1] & 0b00000001) == 0b00000001) {
                            respString += " VNH5019_L";
                        } else if((response[1] & 0b00000010) == 0b00000010) {
                            respString += " VNH5019_R";
                        } else if((response[1] & 0b00000100) == 0b00000100) {
                            respString += " BD1020HFV";
                        } else if((response[1] & 0b00001000) == 0b00001000) {
                            respString += " ML8511A";
                        } else if((response[1] & 0b00010000) == 0b00010000) {
                            respString += " BM1383GLV";
                        } else if((response[1] & 0b00100000) == 0b00100000) {
                            respString += " KX022-1020";
                        } else if((response[1] & 0b01000000) == 0b01000000) {
                            respString += " RPR-0521RS";
                        } else if((response[1] & 0b10000000) == 0b10000000) {
                            respString += " BM1422GMV";
                        }
                        break;
                    default:
                        respString = "CMD_SENSOR_MEASURE - FAIL unknown 0x" + String.format("%02X", response[0] & 0x0F);
                        break;
                }
                break;
            case (byte)CMD_SENSOR_SEND:
                switch(response[0] & 0x0F) {
                    case 0x00:
                        respString = "CMD_SENSOR_SEND - OK";
                        byte[] floatBuff = {response[0], response[1], response[2], response[3]};
                        text_magnetic_induction_x.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[4];
                        floatBuff[1] = response[5];
                        floatBuff[2] = response[6];
                        floatBuff[3] = response[7];
                        text_magnetic_induction_y.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[8];
                        floatBuff[1] = response[9];
                        floatBuff[2] = response[10];
                        floatBuff[3] = response[11];
                        text_magnetic_induction_z.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[14];
                        floatBuff[1] = response[15];
                        floatBuff[2] = response[16];
                        floatBuff[3] = response[17];
                        text_ambient_light.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[18];
                        floatBuff[1] = response[19];
                        floatBuff[2] = response[20];
                        floatBuff[3] = response[21];
                        text_acceleration_x.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[22];
                        floatBuff[1] = response[23];
                        floatBuff[2] = response[24];
                        floatBuff[3] = response[25];
                        text_acceleration_y.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[26];
                        floatBuff[1] = response[27];
                        floatBuff[2] = response[28];
                        floatBuff[3] = response[29];
                        text_acceleration_z.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[30];
                        floatBuff[1] = response[31];
                        floatBuff[2] = response[32];
                        floatBuff[3] = response[33];
                        text_pressure.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[34];
                        floatBuff[1] = response[35];
                        floatBuff[2] = response[36];
                        floatBuff[3] = response[37];
                        text_uv_radiation.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        floatBuff[0] = response[38];
                        floatBuff[1] = response[39];
                        floatBuff[2] = response[40];
                        floatBuff[3] = response[41];
                        text_temperature.setText(Float.toString(ByteBuffer.wrap(floatBuff).order(ByteOrder.LITTLE_ENDIAN).getFloat()));
                        byte[] intBuff = {response[42], response[43]};
                        text_current_left.setText(Float.toString(ByteBuffer.wrap(intBuff).order(ByteOrder.LITTLE_ENDIAN).getInt()));
                        intBuff[0] = response[44];
                        intBuff[1] = response[45];
                        text_current_right.setText(Float.toString(ByteBuffer.wrap(intBuff).order(ByteOrder.LITTLE_ENDIAN).getInt()));
                        break;
                    case 0x01:
                        respString = "CMD_SENSOR_SEND - FAIL";
                        if((response[1] & 0b00000001) == 0b00000001) {
                            respString += " VNH5019_L";
                        } else if((response[1] & 0b00000010) == 0b00000010) {
                            respString += " VNH5019_R";
                        } else if((response[1] & 0b00000100) == 0b00000100) {
                            respString += " BD1020HFV";
                        } else if((response[1] & 0b00001000) == 0b00001000) {
                            respString += " ML8511A";
                        } else if((response[1] & 0b00010000) == 0b00010000) {
                            respString += " BM1383GLV";
                        } else if((response[1] & 0b00100000) == 0b00100000) {
                            respString += " KX022-1020";
                        } else if((response[1] & 0b01000000) == 0b01000000) {
                            respString += " RPR-0521RS";
                        } else if((response[1] & 0b10000000) == 0b10000000) {
                            respString += " BM1422GMV";
                        }
                        break;
                    default:
                        respString = "CMD_SENSOR_SEND - FAIL unknown 0x" + String.format("%02X", response[0] & 0x0F);
                        break;
                }
                break;
            case (byte)CMD_LORA_RESEND_LAST:
                respString = "CMD_LORA_RESEND_LAST - OK";
                break;
            case (byte)CMD_LORA_CONFIG:
                switch(response[0] & 0x0F) {
                    case 0x00:
                        respString = "CMD_LORA_CONFIG - OK";
                        break;
                    case 0x01:
                        respString = "CMD_LORA_CONFIG - FAIL bandwidth invalid";
                        break;
                    case 0x02:
                        respString = "CMD_LORA_CONFIG - FAIL spreading factor invalid";
                        break;
                    case 0x04:
                        respString = "CMD_LORA_CONFIG - FAIL coding rate invalid";
                        break;
                    default:
                        respString = "CMD_LORA_CONFIG - FAIL unknown 0x" + String.format("%02X", response[0] & 0x0F);
                        break;
                }
                break;
            case (byte)CMD_LORA_REQUEST_ERRORS:
                respString = "CMD_LORA_REQUEST_ERRORS - OK";
                break;
            default:
                respString = "CMD_UNKNOWN";
                break;
        }

        text_monitor_output.append("\n");
        text_monitor_output.append(Html.fromHtml("<font color=blue>" + respString + "</font>"));
    }
}
