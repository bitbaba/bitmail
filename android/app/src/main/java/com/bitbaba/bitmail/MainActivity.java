package com.bitbaba.bitmail;

import android.app.Activity;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.os.StrictMode;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends Activity
                          implements View.OnClickListener
                          , ContactsFragment.OnContactsFragmentListener

{

    private ContactsFragment contactsView = new ContactsFragment();

    private SettingsFragment settingsView = new SettingsFragment();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        /**
         * Setup click listener
         */
        TextView btnContacts = (TextView)findViewById(R.id.id_txt_contacts);
        btnContacts.setOnClickListener(this);

        TextView btnSettings = (TextView)findViewById(R.id.id_txt_settings);
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

        TextView txtClicked = (TextView)view;

        switch ( txtClicked.getId() ){
            case R.id.id_txt_contacts:{
                transaction.replace(R.id.id_content, contactsView);
            }break;
            case R.id.id_txt_settings:{
                transaction.replace(R.id.id_content, settingsView);
            }break;
        }

        transaction.commit();
    }

    @Override
    public void onContactsFragmentInteraction(String id) {
        FragmentManager fm = getFragmentManager();
        FragmentTransaction transaction = fm.beginTransaction();
        /**
         * Always create a new chat view;
         */
        ChatFragment chatView = new ChatFragment();
        transaction.replace(R.id.id_content, chatView);
        transaction.commit();
    }
}
