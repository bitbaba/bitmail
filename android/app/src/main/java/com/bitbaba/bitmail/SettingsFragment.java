package com.bitbaba.bitmail;


import android.os.Bundle;
import android.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;

import com.bitbaba.core.BitMail;


/**
 * A simple {@link Fragment} subclass.
 */
public class SettingsFragment extends Fragment  implements CompoundButton.OnCheckedChangeListener{


    public SettingsFragment() {
        // Required empty public constructor
    }


    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View mView = inflater.inflate(R.layout.fragment_settings, container, false);
        TextView tvEmail = (TextView)mView.findViewById(R.id.id_setting_email);
        tvEmail.setText(BitMail.GetInstance().GetEMail());
        TextView tvNick = (TextView)mView.findViewById(R.id.id_setting_nick);
        tvNick.setText(BitMail.GetInstance().GetNick());
        TextView tvTx = (TextView)mView.findViewById(R.id.id_setting_tx);
        tvTx.setText(BitMail.GetInstance().GetTx());
        TextView tvRx = (TextView)mView.findViewById(R.id.id_setting_rx);
        tvRx.setText(BitMail.GetInstance().GetRx());
        TextView tvId = (TextView)mView.findViewById(R.id.id_setting_id);
        tvId.setText(BitMail.GetInstance().GetID());
        CheckBox checkNoSig = (CheckBox) mView.findViewById(R.id.id_check_blockNoSig);
        checkNoSig.setOnCheckedChangeListener(this);
        CheckBox checkStranger = (CheckBox) mView.findViewById(R.id.id_check_blockStranger);
        checkStranger.setOnCheckedChangeListener(this);
        return  mView;
    }

    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
        switch ( compoundButton.getId()){
            case R.id.id_check_blockNoSig:{
                Log.d("BitMail", "block if no signature");
                BitMail.GetInstance().BlockIfNoSig(b);
            }break;
            case R.id.id_check_blockStranger:{
                Log.d("BitMail", "block if strangers");
                BitMail.GetInstance().BlockIfStranger(b);
            }break;
            default:break;
        }
    }
}
