package com.alta.smartwatchapplication.ui.fragments

import android.app.Activity
import android.location.Location
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.activity.addCallback
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.lifecycle.lifecycleScope
import androidx.navigation.fragment.findNavController
import com.alta.smartwatchapplication.databinding.FragmentMapBinding
import com.alta.smartwatchapplication.ui.viewModels.MainViewModel
import com.alta.smartwatchapplication.utils.AppObjs.myDelayClick
import com.alta.smartwatchapplication.utils.contextActivity
import com.google.android.gms.maps.CameraUpdateFactory
import com.google.android.gms.maps.model.LatLng
import com.google.android.gms.maps.model.Marker
import com.google.android.gms.maps.model.MarkerOptions
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch


class MapFragment : Fragment() {
    private val myViewModel: MainViewModel by activityViewModels()
    private var binding: FragmentMapBinding? = null

    var locationLATLON = LatLng(0.0, 0.0)
    var previousMarker: Marker? = null

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?,
    ): View? {
        contextActivity { mContext ->
            binding = FragmentMapBinding.inflate(inflater, container, false)
        }
        return binding?.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        contextActivity { mContext ->
            binding?.apply {

                toolbar.setNavigationOnClickListener {
                    myDelayClick {
                        findNavController().popBackStack()
                    }
                }

                lifecycleScope.launch(Dispatchers.Main) {
                    delay(100)
                    mapView.onCreate(savedInstanceState)
                    mapView.getMapAsync { googleMap ->
                        googleMap.apply {

                            /*appViewModel.mapTYpe.observe(viewLifecycleOwner) {
                                when (it) {
                                    0 -> mapType = GoogleMap.MAP_TYPE_NORMAL
                                    1 -> mapType = GoogleMap.MAP_TYPE_SATELLITE
                                    2 -> mapType = GoogleMap.MAP_TYPE_TERRAIN
                                }
                            }*/

                            myViewModel.locationViewModel.observe(viewLifecycleOwner) {
                                val location = LatLng(
                                    it.latitude,
                                    it.longitude
                                )
                                locationLATLON = location
                                setAddress(mContext, it)
                                previousMarker?.remove()
                                previousMarker = addMarker(MarkerOptions().position(location))
                                moveCamera(CameraUpdateFactory.newLatLngZoom(location, 15f))
                            }

                            mapCurrentDirection.setOnClickListener {
                                previousMarker?.remove()
                                moveCamera(CameraUpdateFactory.newLatLngZoom(locationLATLON, 15f))
                                previousMarker = addMarker(MarkerOptions().position(locationLATLON))
                            }

                            /*googleMap.setOnCameraIdleListener {
                                previousMarker?.remove()
                                previousMarker = addMarker(
                                    MarkerOptions().position(
                                        location
                                    )
                                )
                            }*/
                        }
                    }
                }
            }
        }




        activity?.onBackPressedDispatcher?.addCallback(viewLifecycleOwner) {
            myDelayClick {
                findNavController().popBackStack()
            }
        }
    }

    fun setAddress(mContext: Activity, location: Location) {

    }

    override fun onDestroyView() {
        binding?.mapView?.onDestroy()
        super.onDestroyView()
    }

}
