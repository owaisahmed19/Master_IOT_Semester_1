package com.alta.smartwatchapplication.ui.fragments

import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import com.alta.smartwatchapplication.databinding.FragmentMainBinding
import com.alta.smartwatchapplication.service.BluetoothService
import com.alta.smartwatchapplication.utils.AppObjs.perms

class MainFragment : Fragment() {
    private var binding: FragmentMainBinding? = null

    private val blePermissionLauncher = registerForActivityResult(
        androidx.activity.result.contract.ActivityResultContracts.RequestMultiplePermissions()
    ) { permissions ->
        val allGranted = permissions.entries.all { it.value }
        if (allGranted) {
            startBluetoothService()
        } else {
            binding?.tvStatus?.text = "Permissions denied. Cannot scan."
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?,
    ): View? {
        binding = FragmentMainBinding.inflate(inflater, container, false)
        return binding?.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        binding?.btnScan?.setOnClickListener {
            checkPermissionsAndStart()
        }

        binding?.apply {
            btnScan.setOnClickListener {
                checkPermissionsAndStart()
            }

            btnViewMap.setOnClickListener {
                findNavController().navigate(MainFragmentDirections.actionMainFragmentToMapFragment())
            }
        }
    }

    private fun checkPermissionsAndStart() {
        blePermissionLauncher.launch(perms)
    }

    private fun startBluetoothService() {
        val intent = Intent(requireContext(), BluetoothService::class.java)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            requireContext().startForegroundService(intent)
        } else {
            requireContext().startService(intent)
        }
        binding?.tvStatus?.text = "Service Started: Scanning in background..."
    }
}