package com.alta.smartwatchapplication.ui.fragments

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import com.alta.smartwatchapplication.databinding.FragmentSplashBinding
import com.alta.smartwatchapplication.ui.viewModels.MainViewModel
import com.alta.smartwatchapplication.utils.contextActivity


class SplashFragment : Fragment() {
    private val myViewModel: MainViewModel by activityViewModels()
    private var binding: FragmentSplashBinding? = null

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?,
    ): View? {
        contextActivity { mContext ->
            binding = FragmentSplashBinding.inflate(inflater, container, false)
        }
        return binding?.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        contextActivity { mContext ->
            binding?.apply {


            }
        }
    }
}