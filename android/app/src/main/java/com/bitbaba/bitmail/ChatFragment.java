package com.bitbaba.bitmail;


import android.app.Fragment;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.bitbaba.core.BitMail;

import java.util.List;


/**
 * A simple {@link Fragment} subclass.
 */
public class ChatFragment extends Fragment implements View.OnClickListener
{
    private View mView = null;
    private Button btnSend_ = null;
    private TextView txtMsgs_ = null;
    private EditText editMessage_ = null;

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
        btnSend_ = (Button) mView.findViewById(R.id.id_btn_send);
        txtMsgs_ = (TextView) mView.findViewById(R.id.id_chat_content);
        txtMsgs_.setMovementMethod(new ScrollingMovementMethod());
        editMessage_ = (EditText) mView.findViewById(R.id.id_edit_message);
        btnSend_.setOnClickListener(this);

        /**
         * Populate history messages in Timer
         */
        PopulateMessages();

        return  mView;
    }

    public void PopulateMessages(){
        txtMsgs_.setText("");
        List<String> msgs = BitMail.GetInstance().GetMessage(friendEmail_);
        for (String msg : msgs) {
            txtMsgs_.append(friendEmail_ + ": ");
            txtMsgs_.append(msg);
            txtMsgs_.append("\r\n");
            txtMsgs_.append("\r\n");
        }
    }

    @Override
    public void onClick(View view) {
        Button btnSend = (Button) view;
        switch (btnSend.getId() ){
            case R.id.id_btn_send:{
                onSend();
            }break;
            default:break;
        }
    }

    private void onSend(){
        editMessage_ = (EditText) mView.findViewById(R.id.id_edit_message);
        String txtMsg = editMessage_.getText().toString();
        if (txtMsg.isEmpty()){
            PopulateMessages();
            return;
        }
        // disable before send
        btnSend_.setEnabled(false);
        Log.d("BitMail", txtMsg);
        BitMail.GetInstance().SendMessage(friendEmail_, txtMsg);
        // show what you say.
        PopulateMessages();
        // clear after sent
        editMessage_.setText("");
        // enable after sent
        btnSend_.setEnabled(true);
    }
}
