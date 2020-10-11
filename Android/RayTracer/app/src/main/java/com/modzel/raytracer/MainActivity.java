package com.modzel.raytracer;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {

    GLESView AppView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        AppView = new GLESView(this);
        setContentView(AppView);
    }
}