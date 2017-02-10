package com.bitbaba.bitmail;


import android.os.Bundle;
import android.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;


/**
 * A simple {@link Fragment} subclass.
 */
public class ChatFragment extends Fragment implements View.OnClickListener
{
    private View mView = null;

    private String friendEmail_;

    public ChatFragment() {

    }

    public void SetEmail(String email)
    {
        friendEmail_ = email;
        Log.d("BitMail", friendEmail_);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        mView = inflater.inflate(R.layout.fragment_chat, container, false);
        Button btnSend = (Button) mView.findViewById(R.id.id_btn_send);
        btnSend.setOnClickListener(this);
        return  mView;
    }

    @Override
    public void onClick(View view) {
        Button btnSend = (Button) view;
        btnSend.setEnabled(false);
        EditText editMessage = (EditText) mView.findViewById(R.id.id_edit_message);
        Log.d("BitMail", editMessage.getText().toString());
        editMessage.setText("");
        btnSend.setEnabled(true);
    }
}
