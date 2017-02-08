package com.bitbaba.bitmail;

import android.app.Activity;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.os.StrictMode;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.bitbaba.bitmail.dummy.DummyContent;

public class MainActivity extends Activity
                          implements View.OnClickListener
                          , ContactsFragment.OnListFragmentInteractionListener

{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /**
         * Setup click listener
         */
        Button btnContacts = (Button)findViewById(R.id.id_btn_contacts);
        btnContacts.setOnClickListener(this);

        Button btnSettings = (Button)findViewById(R.id.id_btn_settings);
        btnSettings.setOnClickListener(this);

        /**
         * Access Network in GUI Activity
         */
        StrictMode.setThreadPolicy(new StrictMode.ThreadPolicy.Builder().detectDiskReads()
                .detectDiskWrites().detectNetwork().penaltyLog().build());

        StrictMode.setVmPolicy(new StrictMode.VmPolicy.Builder().detectLeakedSqlLiteObjects()
                .detectLeakedClosableObjects().penaltyLog().penaltyDeath().build());

        /**
         * Setup initial view
         */
        FragmentManager fm = getFragmentManager();
        FragmentTransaction transaction = fm.beginTransaction();
        ContactsFragment contactsView = new ContactsFragment();
        transaction.replace(R.id.id_content, contactsView);
        transaction.commit();

        /**
         * Simple Unit Test
         */
        /*
        try {
            BitMail.GetInstance().UnitTest();
        } catch (IOException e) {
            e.printStackTrace();
        }
        */
    }

    @Override
    public void onClick(View view) {
        FragmentManager fm = getFragmentManager();
        FragmentTransaction transaction = fm.beginTransaction();

        Button btnClicked = (Button)view;

        switch ( btnClicked.getId() ){
            case R.id.id_btn_contacts:{
                ContactsFragment contactsView = new ContactsFragment();
                transaction.replace(R.id.id_content, contactsView);
            }break;
            case R.id.id_btn_settings:{
                SettingsFragment settingsView = new SettingsFragment();
                transaction.replace(R.id.id_content, settingsView);
            }break;
        }

        transaction.commit();
    }

    @Override
    public void onListFragmentInteraction(DummyContent.DummyItem item) {
        Log.d("BitMail", item.toString());

        FragmentManager fm = getFragmentManager();
        FragmentTransaction transaction = fm.beginTransaction();
        ChatFragment chatView = new ChatFragment();
        transaction.replace(R.id.id_content, chatView);
        transaction.commit();
    }
}
