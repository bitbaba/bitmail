package com.bitbaba.bitmail;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import com.bitbaba.core.*;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        X509Cert xcert1 = new X509Cert("nick1", "nick1@example.com", 1024);
        X509Cert xcert2 = new X509Cert("nick2", "nick2@example.com", 1024);

        Log.d("BitMail", xcert1.GetCertificate());
        ArrayList<String> certs = new ArrayList<String>();
        certs.add(xcert1.GetCertificate());
        certs.add(xcert2.GetCertificate());

        //HashMap<String, String> result = xcert1.Verify(xcert1.Decrypt(xcert1.Encrypt(xcert1.Sign("Hello, World"))));
        HashMap<String, String> result = xcert1.Verify(xcert1.Decrypt(X509Cert.MEncrypt(certs, xcert1.Sign("Hello, World"))));

        if (result != null){
            for (Entry<String, String> elt : result.entrySet()){
                Log.d("BitMail", elt.getKey());
                Log.d("BitMail", elt.getValue());
            }
        }
    }
}
