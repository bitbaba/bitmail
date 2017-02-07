package com.bitbaba.bitmail;

import android.app.Activity;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.os.StrictMode;

public class MainActivity extends Activity
                          //implements ContactFragment.OnListFragmentInteractionListener, MessageFragment.OnListFragmentInteractionListener, SettingFragment.OnListFragmentInteractionListener
{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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

    /*
    @Override
    public void onListFragmentInteraction(DummyContent.DummyItem item) {

    }
    */
}
